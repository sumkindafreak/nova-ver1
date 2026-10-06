#pragma once

#include <Arduino.h>
#include "NovaTypes.h"

class NovaMotion {
 public:
  static NovaJointSet solveTargets(const NovaFootTargets& targets);
  static void printSolvedTargets(Stream& out,
                                 const NovaFootTargets& targets);
};
