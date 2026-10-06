# Nova kinematics

Nova now has a lightweight ESP32-native inverse-kinematics layer.

It deliberately does **not** depend on ROS, micro-ROS or Eigen.

## Coordinate system

- +X = forward
- +Y = up
- +Z = robot's right

All internal kinematics distances are stored in metres.

## Reference geometry

The initial software geometry is based on the SpotMicro frame family used by Thingiverse 3445283:

| Item | Initial value |
| --- | ---: |
| Hip link | 55.0 mm |
| Upper leg | 107.5 mm |
| Lower leg | 130.0 mm |
| Front/rear hip span | 186.0 mm |
| Left/right hip span | 78.0 mm |
| Stand height | 155.0 mm |

These values are marked as **not physically confirmed** in firmware.

Before kinematic output is permitted to command the servos we must measure Nova's actual printed mechanism and calibrate all twelve joints.

## Software path

```text
Pose / gait
   |
   v
four foot XYZ targets
   |
   v
NovaMotion
   |
   v
NovaKinematics
   |
   v
12 kinematic joint angles
   |
   v
future calibrated servo mapper
   |
   v
PCA9685
```

The final servo-mapper path is currently safety-locked.

## Serial maths tests

```text
geometry
stancecalc
ik FR 108 -155 94
crawl 0 0
crawl 1 50
crawl 1 100
```

The crawl gait alternates body-shift and single-leg-swing phases so only one foot is lifted at a time.

## Safety gate

`NOVA_KINEMATIC_SERVO_OUTPUT_ENABLED` is currently `false`.

Before enabling automatic motion complete:

1. physical link measurements,
2. channel identification,
3. centre-angle calibration,
4. direction/reversal calibration,
5. safe mechanical min/max calibration,
6. supported stand testing.
