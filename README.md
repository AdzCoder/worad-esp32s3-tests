# 🐾 worad-esp32-tests

> ESP32-S3 hardware test firmware for the **WORAD robot dog** — ES410 Group 5, University of Warwick (2025/26).

A single PlatformIO project with multiple environments covering CAN bus communication, brushless motor control, and time-of-flight distance sensing. All targeting the **Adafruit ESP32-S3 Feather**.

---

## 📦 Environments

| Environment  | Description                                | Key Hardware                 |
| ------------ | ------------------------------------------ | ---------------------------- |
| `can_test`   | TWAI CAN bus transmit/receive test         | ESP32-S3 TWAI peripheral     |
| `motor_test` | CubeMars AK45-36 position control over CAN | AK45-36 @ 1Mbps CAN          |
| `tof_sensor` | VL53L4CX continuous distance measurement   | VL53L4CX via STEMMA QT / I2C |

---

## 🛠️ Build & Upload

Requires [PlatformIO](https://platformio.org/).

```bash
# Build a specific environment
pio run -e can_test

# Upload to board
pio run -e motor_test --target upload

# Open serial monitor
pio device monitor -e tof_sensor
```

---

## 📁 Project Structure

```
worad-esp32-tests/
├── platformio.ini
└── src/
    ├── can_test/
    │   └── main.cpp
    ├── motor_test/
    │   └── main.cpp
    └── tof_sensor/
        └── main.cpp
```

---

## ⚙️ Hardware

- **Board:** Adafruit ESP32-S3 Feather (no PSRAM)
- **CAN transceiver:** Connected on GPIO 10 (RX) / GPIO 11 (TX)
- **ToF sensor:** VL53L4CX via STEMMA QT (I2C, I2C power on GPIO 7)
- **Motor:** CubeMars AK45-36, extended CAN frame, 1Mbps

---

## 🔗 Part of

[WORAD ES410 Group 5 — University of Warwick, School of Engineering](https://warwick.ac.uk)

---

_Testing code — not production firmware._
