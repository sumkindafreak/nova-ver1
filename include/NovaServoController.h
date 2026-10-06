#pragma once

#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>

#include "NovaTypes.h"

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

  bool begin();
  void disableAll();
  bool setServoAngle(uint8_t servoIndex, float angleDeg);

  bool setLegAngles(NovaLeg leg,
                    float hipDeg,
                    float upperDeg,
                    float lowerDeg);

  void neutralAll();
  bool isEnergized() const;
  void printMap(Stream& out) const;

 private:
  Adafruit_PWMServoDriver pwm_;
  bool energized_;

  uint16_t angleToTicks(const NovaServoCalibration& calibration,
                        float requestedAngleDeg) const;
};
