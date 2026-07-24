# ESP32 DevKit Upload Starter

This repository contains a minimal Arduino sketch and upload guide for an ESP32 DevKit.

## Why this was added
The shared `chatgpt.com` link cannot be fetched from this environment (HTTP 403), so this starter gives you a working base you can flash now and then replace with the code from your link.

## Files
- `esp32_devkit/esp32_devkit.ino` — starter sketch with clear section to paste your custom logic.

## Upload via Arduino IDE
1. Connect ESP32 DevKit with a **data USB cable**.
2. Install **Arduino IDE**.
3. Add ESP32 board URL in Arduino IDE:
   - `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
4. Install **esp32 by Espressif Systems** via Boards Manager.
5. Select board: `Tools > Board > ESP32 Arduino > ESP32 Dev Module`.
6. Select serial port in `Tools > Port`.
7. Open `esp32_devkit/esp32_devkit.ino` and click **Upload**.
8. If upload is stuck on "Connecting...", hold **BOOT** while upload starts, then release.
9. Open Serial Monitor at **115200** baud.

## Next step
Paste the exact code from your ChatGPT shared link into the marked `YOUR FEATURE START/END` section, then upload again.


## PPE website concept
This repository now includes a static marketing website concept for PPE product sales and digital presence. Open `index.html` in a browser to review the landing page, product category sections, industry positioning, and quote enquiry form.

## Website files
- `index.html` — responsive PPE sales landing page.
- `styles.css` — visual design, layout, and responsive styles for the website.
