# ESP32 Beacon Flood (PlatformIO, Chromebook-friendly)

This is a **clean template** for the ESP32 SSID beacon flood sketch. Edit your SSIDs privately in `src/main.cpp` and build a **.bin** firmware you can flash from your Chromebook using the browser.

## Quick Start (Gitpod: build in your browser)

1. Create a **private GitHub repo** and upload this folder's files.
2. Open: `https://gitpod.io/#<your-repo-url>`
3. In the Gitpod terminal, run:
   ```bash
   pio run -e esp32dev
   ```
4. When it finishes, your firmware will be at:
   ```
   .pio/build/esp32dev/firmware.bin
   ```
5. Download that `.bin` to your Chromebook.

## Flash from Chromebook

Use the web flasher:
- https://web.esphome.io/
- Click **Connect** → choose your ESP32 port
- Click **Install** → **Custom .bin** → pick the file you downloaded
- Wait for success; the ESP32 will start broadcasting immediately.

## Edit SSIDs

Open `src/main.cpp` and replace the sample entries in:
```cpp
const char* ssidList[] = { "SSID_1", "SSID_2", ... };
```
Max 32 characters per SSID. Add lines freely; `ssidCount` is computed automatically.

## Notes

- Board: **ESP32 Dev Module** (`esp32dev`)
- Framework: Arduino
- Serial monitor speed: `115200`
- TX power is pushed close to max (`esp_wifi_set_max_tx_power(78)`)
- Channel fixed to `6` for visibility; you can change it.

**You are responsible for how you use this. Use only in a controlled environment.**