# Nova reference material

Nova is its own ESP32-S3 firmware project, but these open projects and research have been useful references for geometry, inverse kinematics and gait architecture.

## SpotMicro references

- antonioasaro/spotmicro_ws
  - ESP32 / ROS2 / micro-ROS SpotMicro implementation
  - useful reference for frame geometry, servo calibration concepts and crawl gait structure
- mike4192/spotMicro
  - SpotMicro quadruped control project
  - useful reference for inverse kinematics, poses and the stable 8-phase walking approach
- Thingiverse 3445283
  - SpotMicro printable mechanical frame family used by Nova

Nova does not vendor ROS, micro-ROS or Eigen source trees from these projects.

## Kinematics paper

The SpotMicro projects cite:

M. A. Sen, V. Bakircioglu and M. Kalyoncu,
"Inverse Kinematic Analysis Of A Quadruped Robot",
International Journal of Scientific & Technology Research, 2017.

## Licensing

The referenced SpotMicro kinematics package identifies its software as MIT licensed.

See `THIRD_PARTY_NOTICES.md`.
