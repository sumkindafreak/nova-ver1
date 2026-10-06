#pragma once

#include <Arduino.h>

// Shared Nova data types.
// Coordinate system:
//   +X = forward
//   +Y = up
//   +Z = robot's right
// Distances are metres unless a function explicitly says otherwise.

enum class NovaLeg : uint8_t {
  FrontLeft = 0,
  FrontRight = 1,
  RearLeft = 2,
  RearRight = 3
};

struct NovaPoint3 {
  float x;
  float y;
  float z;
};

struct NovaJointAngles {
  float hipRad;
  float shoulderRad;
  float kneeRad;
  bool valid;
};

struct NovaFootTargets {
  NovaPoint3 frontLeft;
  NovaPoint3 frontRight;
  NovaPoint3 rearLeft;
  NovaPoint3 rearRight;
};

struct NovaJointSet {
  NovaJointAngles frontLeft;
  NovaJointAngles frontRight;
  NovaJointAngles rearLeft;
  NovaJointAngles rearRight;
  bool valid;
};

inline const char* novaLegName(NovaLeg leg) {
  switch (leg) {
    case NovaLeg::FrontLeft: return "FL";
    case NovaLeg::FrontRight: return "FR";
    case NovaLeg::RearLeft: return "RL";
    case NovaLeg::RearRight: return "RR";
  }
  return "??";
}
