# Nova v1 hardware notes

## Mechanical layout

Nova is a headless quadruped based around a Spot-style body.

- 4 legs
- 3 servo joints per leg
- 12 servos total
- front-mounted ToF distance sensor
- body-mounted IMU / gyroscope

The front ToF module is part of the body rather than a head.

## Controller architecture

The ESP32-S3 is the main processor.

The PCA9685 provides stable hardware PWM for all 12 servos and leaves four channels free.

The IMU and ToF sensor share the I2C bus with the PCA9685, provided their addresses do not conflict.

### Initial I2C wiring

| Signal | ESP32-S3 |
| --- | --- |
| SDA | GPIO 8 |
| SCL | GPIO 9 |
| 3V3 | Logic/sensor supply where appropriate |
| GND | Common ground |

### PCA9685

| Item | Initial value |
| --- | --- |
| I2C address | 0x40 |
| PWM frequency | 50 Hz |
| Used channels | 0-11 |
| Spare channels | 12-15 |

## Initial channel allocation

| Servo index | PCA9685 channel | Position |
| ---: | ---: | --- |
| 0 | 0 | Front Left hip |
| 1 | 1 | Front Left upper |
| 2 | 2 | Front Left lower |
| 3 | 3 | Front Right hip |
| 4 | 4 | Front Right upper |
| 5 | 5 | Front Right lower |
| 6 | 6 | Rear Left hip |
| 7 | 7 | Rear Left upper |
| 8 | 8 | Rear Left lower |
| 9 | 9 | Rear Right hip |
| 10 | 10 | Rear Right upper |
| 11 | 11 | Rear Right lower |

This map is the software starting point. It should be changed to match the final physical harness if the build is wired differently.

## Power architecture

Twelve servos can draw very large transient currents.

The servo rail must therefore use a dedicated power supply sized for the actual servo model. Do not run the servo rail from:

- ESP32-S3 5 V,
- ESP32-S3 3.3 V,
- USB.

The PCA9685 logic side and sensors can be powered separately from the servo rail as appropriate, but all control electronics must share a common ground reference.

A large electrolytic capacitor close to the PCA9685 servo rail can help absorb short transients, but it is not a substitute for a correctly sized supply and wiring.

## First power-up procedure

1. Leave the servo power supply disconnected.
2. Power only the ESP32-S3 and PCA9685 logic.
3. Open Serial at 115200 baud.
4. Run `scan`.
5. Confirm the PCA9685 appears at 0x40.
6. Connect the IMU and ToF modules one at a time and record their addresses.
7. Disconnect USB/control power.
8. Connect the external servo supply with Nova physically supported.
9. Restart.
10. Test one servo at a time near 90 degrees.
11. Record direction, safe minimum angle, safe maximum angle and mechanical centre for every joint.

Do not run `neutral` until individual channels have been checked well enough that a 90 degree command cannot drive a linkage into a hard stop.

## Sensor integration

The Phase 0 firmware performs an address scan rather than assuming a specific IMU or ToF module.

Common addresses are displayed as hints:

- 0x68 / 0x69: common IMU addresses
- 0x29: common ST VL53-family ToF address

Once the exact modules are confirmed, their real drivers will be added and tested independently before they are used by gait or balance code.
