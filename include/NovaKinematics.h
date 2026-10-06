#pragma once

#include <Arduino.h>
#include "NovaTypes.h"

class NovaKinematics {
 public:
  static NovaPoint3 bodyToLegLocal(NovaLeg leg, const NovaPoint3& bodyPoint);
  static NovaJointAngles solveLegLocal(NovaLeg leg, const NovaPoint3& localPoint);
  static NovaJointAngles solveBodyFoot(NovaLeg leg, const NovaPoint3& bodyPoint);
  static NovaPoint3 neutralFootBody(NovaLeg leg);
  static NovaJointSet solveNeutralStance();

  static void printGeometry(Stream& out);
  static void printSolution(Stream& out,
                            NovaLeg leg,
                            const NovaPoint3& bodyPoint,
                            const NovaJointAngles& angles);
};
