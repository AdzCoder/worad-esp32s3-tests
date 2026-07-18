# WORAD ESP32-S3 Tests

[![ESP32-S3](https://img.shields.io/badge/ESP32--S3-Feather-blue?style=flat-square)](https://www.adafruit.com/product/5323)
[![PlatformIO](https://img.shields.io/badge/PlatformIO-CI-orange?style=flat-square)](https://platformio.org/)
[![University](https://img.shields.io/badge/University-Warwick-green?style=flat-square)](https://warwick.ac.uk/)

Hardware bring-up firmware for the **WORAD** robot dog, targeting the Adafruit ESP32-S3 Feather. This is a single PlatformIO project holding small, self-contained test programs used to verify individual subsystems (CAN bus, motor control) before they are integrated into the full robot. It is deliberately scrappy: bench test code, not production firmware.

## Background

WORAD is the Warwick Olfactory Robotic Assistance Dog, a fourth-year group project (ES410, 2025/26) supervised by Professor James Covington. The goal is a quadruped robot that detects and classifies gases and odours, aimed at applications such as public safety, environmental monitoring, and search and rescue. It builds on the previous year's platform, adding a chemical-sensor suite and the analysis to act on it. This repository is the low-level firmware used to prove out the electronics and actuation on the ESP32-S3 before higher-level behaviour is layered on.

## Environments

| Environment | What it tests | Key hardware |
|-------------|---------------|--------------|
| `can_test` | TWAI CAN bus transmit and receive | ESP32-S3 TWAI peripheral |
| `motor_test` | CubeMars AK45-36 position control over CAN | AK45-36 at 1 Mbps |

A `tof_test` environment for the VL53L4CX time-of-flight sensor is configured in `platformio.ini` but is excluded from the default build until its source is added under `src/tof_test/`.

## Build and upload

Requires [PlatformIO](https://platformio.org/).

```bash
pio run -e can_test                        # build one environment
pio run -e motor_test --target upload      # upload to the board
pio device monitor -e can_test             # serial monitor
```

Running `pio run` with no environment builds `can_test` and `motor_test`.

## Hardware

- **Board:** Adafruit ESP32-S3 Feather (no PSRAM)
- **CAN transceiver:** GPIO 10 (RX) / GPIO 11 (TX)
- **Motor:** CubeMars AK45-36, extended CAN frame, 1 Mbps

## Repository layout

```
worad-esp32s3-tests/
├── platformio.ini
└── src/
    ├── can_test/
    │   └── main.cpp
    └── motor_test/
        └── main.cpp
```

## Part of

WORAD, ES410 Group 5, University of Warwick, School of Engineering.

---

*Testing code, not production firmware.*
