#pragma once

#include "NovaTypes.h"

class NovaPose {
 public:
  static NovaFootTargets stance(float bodyHeightM);
  static NovaFootTargets stand();
  static NovaFootTargets crouch();
};
