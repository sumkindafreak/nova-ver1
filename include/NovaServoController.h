#pragma once

#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>

enum class NovaLeg : uint8_t {
  FrontLeft = 0,
  FrontRight = 1,
  RearLeft = 2,
  RearRight = 3
};

struct NovaServoCalibration {
  uint8_t channel;
  uint16_t minPulseUs;
  uint16_t maxPulseUs;
  bool reversed;
  float offsetDeg;
};

class NovaServoController {
 public:
  NovaServoController();

  // Initialise the PCA9685. Outputs remain disabled after begin().
  bool begin();

  // Immediately switch every PCA9685 channel fully off.
  void disableAll();

  // Move one of Nova's 12 configured joints.
  bool setServoAngle(uint8_t servoIndex, float angleDeg);

  // Move all three joints belonging to one leg.
  bool setLegAngles(NovaLeg leg,
                    float hipDeg,
                    float upperDeg,
                    float lowerDeg);

  // Move all 12 configured servos to the neutral test angle.
  void neutralAll();

  bool isEnergized() const;

  // Print the current logical channel/calibration map.
  void printMap(Stream& out) const;

 private:
  Adafruit_PWMServoDriver pwm_;
  bool energized_;

  uint16_t angleToTicks(const NovaServoCalibration& calibration,
                        float requestedAngleDeg) const;
};
