# Adaptive Presence Light (ESP32)

Proximity and motion-aware RGB strip controller for ESP32 with:

- HC-SR04 distance measurement + EMA smoothing (optional Kalman)
- PIR-triggered occupancy state machine (ACTIVE/IDLE/SLEEP)
- Linear distance-to-color interpolation (red near, blue far)
- 60 FPS fade engine with optional gamma correction
- LDR-driven adaptive brightness
- FreeRTOS task separation (`sensor_task`, `lighting_task`, `wifi_task`, `power_task`)
- Optional WiFi dashboard for telemetry and manual override

## Files

- `config.h`: pin mapping, timing, thresholds, and feature toggles.
- `sensor.*`: ultrasonic + PIR + LDR acquisition and filtering.
- `lighting.*`: color mapping, fade interpolation, and PWM output.
- `state_machine.*`: ACTIVE/IDLE/SLEEP transitions.
- `power.*`: sleep mode hooks.
- `wifi.*`: ESP32 web server endpoints.
- `main.ino`: startup and RTOS task orchestration.

## Dashboard endpoints

- `/` status page (distance/state/mode)
- `/auto?enabled=1|0`
- `/color?r=255&g=64&b=0`

## Notes

- Add a voltage divider on `ECHO` (5V -> 3.3V).
- Replace WiFi placeholders in `wifi.cpp` (`kSsid`, `kPass`).
- `IRFZ44N` may require gate driver for full enhancement at 3.3V logic depending on strip current.

## GitHub Actions quickstart

A starter workflow is included at `.github/workflows/github-actions-demo.yml`.

- Trigger: every `push`
- Job: `Explore-GitHub-Actions` on `ubuntu-latest`
- Purpose: validate Actions setup and show basic workflow step output

You can view results in the repository **Actions** tab after pushing commits.

## Auto-download + IDE-ready structuring

Use `github_to_arduino.py` to clone a GitHub project and prepare a clean Arduino IDE sketch folder automatically.

Example:

```bash
python github_to_arduino.py https://github.com/your-user/your-esp32-project.git --ref main --project-name AdaptivePresenceLight --output-dir downloads
```

What it does:

- clones the repo (shallow)
- finds a primary `.ino` (`<repo>.ino`, `main.ino`, or first discovered)
- copies only the sketch folder contents into `downloads/<project-name>`
- renames the main sketch file to `<project-name>.ino` for Arduino IDE compatibility
- writes `UPLOAD_NOTES.md` with upload steps
