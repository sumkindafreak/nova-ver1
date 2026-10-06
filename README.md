# Nova v1

Nova v1 is a rebuild of Nova as a compact, headless, Spot-style quadruped robot.

The mechanical platform has **four legs with three joints per leg** (12 servos total). A front-facing Time-of-Flight sensor provides obstacle/distance sensing, while an IMU/gyroscope will provide body attitude and motion feedback.

## Planned hardware

- ESP32-S3 main controller
- PCA9685 16-channel PWM servo controller
- 12 servos: 3 joints per leg
- IMU / gyroscope over I2C
- Front-facing Time-of-Flight sensor over I2C
- Separate high-current servo power supply
- Common ground between servo supply, PCA9685 and ESP32-S3

There is deliberately **no robot head** in this build. The ToF sensor sits at the front of the body.

## Current firmware stage

The repository currently contains the **Phase 0 bench bring-up firmware**.

It is intentionally conservative:

- Servo outputs start disabled.
- The PCA9685 is initialised at 50 Hz.
- The I2C bus is scanned at boot so connected devices can be identified.
- Individual servos can be moved manually from Serial.
- A complete leg can be positioned from Serial.
- All 12 joints can be moved to a neutral 90 degree test position.
- All PWM outputs can immediately be disabled.

This gives us a safe way to confirm wiring, channel order and joint direction before adding calibration, inverse kinematics or walking gaits.

## Quick start

This project is set up for PlatformIO using the Arduino framework.

1. Open the repository in VS Code with PlatformIO installed.
2. Connect the ESP32-S3 by USB.
3. Build and upload the `esp32-s3-devkitc-1` environment.
4. Open the Serial Monitor at **115200 baud**.
5. Keep Nova supported with all feet clear of the bench while testing.

The default I2C pins are:

| Signal | ESP32-S3 GPIO |
| --- | ---: |
| SDA | 8 |
| SCL | 9 |

The PCA9685 default address is `0x40`.

If your exact ESP32-S3 board uses different pins, change them in `include/NovaConfig.h`.

## Servo channel map

| Leg | Hip | Upper leg | Lower leg |
| --- | ---: | ---: | ---: |
| Front Left | 0 | 1 | 2 |
| Front Right | 3 | 4 | 5 |
| Rear Left | 6 | 7 | 8 |
| Rear Right | 9 | 10 | 11 |

PCA9685 channels 12-15 are currently spare.

## Serial commands

```text
help
scan
status
neutral
disable
servo <channel> <angle>
leg <FL|FR|RL|RR> <hip> <upper> <lower>
```

Examples:

```text
servo 0 90
servo 5 110
leg FL 90 80 100
neutral
disable
```

Angles are limited to 0-180 degrees. The initial pulse range is deliberately conservative and must be calibrated to the actual servos and printed geometry before Nova is allowed to walk.

## Safety

Do **not** power twelve servos from the ESP32-S3 or USB supply.

Use a correctly rated external servo supply connected to the PCA9685 servo power rail. The ESP32-S3, PCA9685 and servo supply must share ground.

During initial calibration:

- keep the robot supported,
- keep feet clear of the bench,
- test one channel at a time,
- use small movements,
- be ready to disconnect servo power.

## Repository layout

```text
nova-ver1/
├── include/
│   ├── NovaConfig.h
│   └── NovaServoController.h
├── src/
│   ├── main.cpp
│   └── NovaServoController.cpp
├── docs/
│   ├── HARDWARE.md
│   └── ROADMAP.md
├── platformio.ini
└── README.md
```

## Build direction

Nova will be developed in small testable stages:

1. Electrical and servo bring-up
2. Per-joint calibration and direction mapping
3. IMU and ToF driver integration
4. Body geometry model
5. Inverse kinematics
6. Safe stand / sit poses
7. Static balance testing
8. Slow crawl gait
9. Trot and smoother gait generation
10. IMU-assisted attitude correction and ToF obstacle behaviour

See `docs/ROADMAP.md` for the working plan.
