# Nova v1 development roadmap

The rule: **prove each layer on the bench before the next layer depends on it**.

## Phase 0 - controller bring-up

- [x] ESP32-S3 PlatformIO project
- [x] PCA9685 driver
- [x] 12-joint logical map
- [x] outputs disabled at boot
- [x] manual single-servo command
- [x] manual whole-leg command
- [x] I2C scanner
- [ ] confirm actual SDA/SCL pins
- [ ] confirm PCA9685 address
- [ ] confirm servo channel wiring

## Phase 1 - joint calibration

For all 12 joints record channel, direction, safe min/max, physical centre and software offset.

- [ ] Front Left
- [ ] Front Right
- [ ] Rear Left
- [ ] Rear Right

Exit: logical joint angles produce predictable safe movement.

## Phase 2 - geometry and IK

- [x] body coordinate convention
- [x] SpotMicro reference geometry
- [x] body-to-leg conversion
- [x] 3-DOF inverse kinematics
- [x] four-leg solver
- [x] neutral stance solver
- [x] unreachable-target rejection
- [x] Serial IK preview
- [ ] measure actual hip link
- [ ] measure actual upper leg
- [ ] measure actual lower leg
- [ ] measure hip-to-hip length
- [ ] measure hip-to-hip width
- [ ] mark geometry confirmed
- [ ] compare calculated angles with real supported legs

## Phase 3 - safe poses

- [x] software stand pose
- [x] software crouch pose
- [ ] calibrated IK-angle to servo-angle mapper
- [ ] rate-limited interpolation
- [ ] supported crouch test
- [ ] supported stand test
- [ ] sit/rest pose

## Phase 4 - first crawl

- [x] 8-phase crawl layout
- [x] alternating body-shift and swing phases
- [x] only one swing leg at a time
- [x] triangular foot lift
- [x] Serial crawl preview
- [ ] apply body shift to target transform
- [ ] connect gait targets to calibrated servo output
- [ ] graceful gait start/stop
- [ ] supported slow-motion gait test
- [ ] flat-floor crawl test

## Phase 5 - sensors

IMU:
- [ ] identify exact module
- [ ] accelerometer
- [ ] gyro
- [ ] roll/pitch
- [ ] stationary bias calibration
- [ ] filtering

ToF:
- [ ] identify exact module
- [ ] front distance
- [ ] validity
- [ ] filtering
- [ ] obstacle threshold

## Phase 6 - balance

- [ ] feed IMU roll/pitch into body state
- [ ] tiny four-feet-planted corrections
- [ ] correction clamps
- [ ] damping/tuning

## Phase 7 - obstacle behaviour

- [ ] slow near obstacle
- [ ] stop at safety distance
- [ ] simple turn-away behaviour

## Phase 8 - smoother locomotion

- [ ] better interpolation
- [ ] acceleration/deceleration
- [ ] turning
- [ ] lateral movement
- [ ] body-height changes
- [ ] optional diagonal trot

## Phase 9 - operator interface

- [ ] Wi-Fi control
- [ ] WebUI
- [ ] telemetry
- [ ] calibration editor
- [ ] gait tuning
- [ ] OTA
