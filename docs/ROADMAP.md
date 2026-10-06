# Nova v1 development roadmap

The guiding rule for Nova is simple: **prove each layer on the bench before the next layer depends on it**.

## Phase 0 - controller bring-up

Current phase.

- [x] Create clean Nova v1 repository
- [x] ESP32-S3 PlatformIO project
- [x] PCA9685 driver layer
- [x] 12-joint logical map
- [x] Outputs disabled at boot
- [x] Manual single-servo command
- [x] Manual whole-leg command
- [x] I2C scanner
- [ ] Confirm ESP32-S3 SDA/SCL pins on the actual board
- [ ] Confirm PCA9685 address
- [ ] Confirm servo channel wiring

Exit condition: every connected device is visible and each servo channel can be identified safely.

## Phase 1 - joint calibration

For all 12 joints record:

- electrical channel
- physical joint name
- direction
- safe minimum angle
- safe maximum angle
- physical centre
- software offset

Then replace the temporary shared calibration values with per-joint values.

Exit condition: a requested logical angle produces predictable motion without hitting a hard stop.

## Phase 2 - sensors

Add the exact IMU and ToF drivers after their modules are identified.

IMU goals:

- accelerometer readings
- gyro readings
- roll/pitch estimate
- calibration
- stationary bias measurement

ToF goals:

- front distance in millimetres
- range validity
- filtering
- configurable obstacle threshold

Exit condition: stable timestamped sensor data is available without disturbing servo updates.

## Phase 3 - geometry model

Measure the printed robot and define:

- hip offsets relative to body centre
- upper leg length
- lower leg length
- nominal standing height
- usable foot workspace

Exit condition: the software model matches the real mechanism.

## Phase 4 - inverse kinematics

Create a leg IK solver that converts a target foot position into three joint angles.

Required protections:

- reject unreachable targets
- clamp to calibrated joint limits
- never emit NaN/invalid servo commands
- log rejected targets during development

Exit condition: all four supported legs can trace slow test paths while Nova is off the ground.

## Phase 5 - poses

Implement named body states:

- safe/rest
- sit
- stand
- crouch
- calibration pose

Transitions must be interpolated rather than instant.

Exit condition: Nova can repeatedly move from rest to stand and back while supported.

## Phase 6 - static balance

Use the IMU to observe body roll and pitch.

First test only tiny corrections while all four feet remain planted.

Exit condition: Nova can compensate for small body attitude changes without oscillating.

## Phase 7 - first gait

Start with a slow crawl gait.

Priorities:

1. stability
2. repeatability
3. current draw
4. graceful stop
5. speed

Exit condition: Nova can take repeated controlled steps on a flat surface.

## Phase 8 - smoother motion

After crawl is reliable:

- gait interpolation
- acceleration/deceleration ramps
- turning
- lateral movement
- body height changes
- optional trot

## Phase 9 - environmental behaviour

Use the front ToF sensor for simple behaviour such as:

- slow down near obstacles
- stop below a safety distance
- choose a turn direction

This layer must never bypass lower-level joint and safety limits.

## Phase 10 - control interface

Once locomotion itself is reliable, add the preferred operator interface.

Possible features:

- Wi-Fi control
- WebUI
- telemetry
- battery/current information
- calibration editor
- gait tuning
- OTA firmware updates

The control interface comes after reliable locomotion, not before it.
