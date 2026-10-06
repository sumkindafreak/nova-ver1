# Nova v1

Nova v1 is a rebuild of Nova as a compact, headless, SpotMicro-style quadruped robot using an ESP32-S3.

Nova has **four legs with three joints per leg** (12 servos total). A front-facing Time-of-Flight sensor will provide obstacle/distance sensing, while an IMU/gyroscope will provide body attitude and motion feedback.

There is deliberately **no robot head**. The ToF sensor sits at the front of the body.

## Hardware

- ESP32-S3 main controller
- PCA9685 16-channel PWM servo controller
- 12 servos
- IMU / gyroscope over I2C
- Front-facing ToF sensor over I2C
- Separate high-current servo supply
- Common ground between servo supply, PCA9685 and ESP32-S3

## Firmware status

### Bench-control layer

- PCA9685 at 50 Hz
- servo outputs disabled at boot
- I2C discovery
- individual servo movement
- complete three-joint leg movement
- direct neutral 90-degree bench command
- immediate output disable

### Motion-maths layer

- Thingiverse/SpotMicro reference geometry
- body-to-leg coordinate conversion
- 3-DOF inverse kinematics
- four-foot pose representation
- neutral stand pose
- crouch pose
- whole-body target solver
- safe 8-phase crawl-gait preview
- body-shift targets between single-leg swing phases

The motion-maths layer is currently **calculation only**.

## Safety lock

```cpp
NOVA_KINEMATIC_SERVO_OUTPUT_ENABLED = false
```

That remains locked until Nova's actual printed geometry and all twelve servo centres/directions are confirmed.

## Quick start

1. Open the repo in VS Code + PlatformIO.
2. Connect the ESP32-S3.
3. Build/upload `esp32-s3-devkitc-1`.
4. Open Serial at **115200 baud**.
5. Keep Nova supported during servo testing.

Default I2C:

| Signal | GPIO |
| --- | ---: |
| SDA | 8 |
| SCL | 9 |

PCA9685 address: `0x40`.

## Servo map

| Leg | Hip | Upper | Lower |
| --- | ---: | ---: | ---: |
| Front Left | 0 | 1 | 2 |
| Front Right | 3 | 4 | 5 |
| Rear Left | 6 | 7 | 8 |
| Rear Right | 9 | 10 | 11 |

Channels 12-15 are spare.

## Serial commands

### Direct bench control

```text
help
scan
status
neutral
disable
servo <channel> <angle>
leg <FL|FR|RL|RR> <hip> <upper> <lower>
```

### Kinematics / gait preview

```text
geometry
stancecalc
ik <FL|FR|RL|RR> <x_mm> <y_mm> <z_mm>
crawl <phase 0-7> <progress 0-100>
```

Examples:

```text
stancecalc
ik FR 108 -155 94
crawl 0 0
crawl 1 50
crawl 1 100
```

These preview commands **do not move the servos**.

## Reference geometry

| Item | Value |
| --- | ---: |
| Hip link | 55.0 mm |
| Upper leg | 107.5 mm |
| Lower leg | 130.0 mm |
| Front/rear hip span | 186.0 mm |
| Left/right hip span | 78.0 mm |
| Stand height | 155.0 mm |

## Repository structure

```text
include/
  NovaConfig.h
  NovaTypes.h
  NovaGeometry.h
  NovaKinematics.h
  NovaPose.h
  NovaMotion.h
  NovaGait.h
  NovaServoController.h

src/
  main.cpp
  NovaKinematics.cpp
  NovaPose.cpp
  NovaMotion.cpp
  NovaGait.cpp
  NovaServoController.cpp

docs/
  HARDWARE.md
  KINEMATICS.md
  REFERENCES.md
  ROADMAP.md
```

## Development order

1. Bench electrical/PWM bring-up
2. Calibrate all 12 joints
3. Confirm printed geometry
4. Validate IK while supported
5. Connect calibrated IK to servo output
6. Interpolated sit/stand
7. Slow 8-phase crawl
8. IMU attitude correction
9. ToF obstacle behaviour
10. Faster gait only after crawl is dependable

See `docs/ROADMAP.md`.

## References

Nova's motion architecture was informed by open SpotMicro projects, especially `antonioasaro/spotmicro_ws` and `mike4192/spotMicro`.

Nova remains standalone ESP32-S3 firmware and does not require ROS2, micro-ROS or Eigen.
