#pragma once

#include <Arduino.h>
#include "NovaTypes.h"

struct NovaGaitFrame {
  NovaFootTargets feet;
  NovaPoint3 bodyShift;
  uint8_t phase;
  float progress;
  bool swingActive;
  NovaLeg swingLeg;
};

class NovaGait {
 public:
  static NovaGaitFrame sampleCrawl(uint8_t phase, float progress);
  static void printFrame(Stream& out, const NovaGaitFrame& frame);
};
