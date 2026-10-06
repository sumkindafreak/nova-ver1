#include "NovaMotion.h"

#include "NovaKinematics.h"

NovaJointSet NovaMotion::solveTargets(const NovaFootTargets& targets) {
  NovaJointSet result;

  result.frontLeft = NovaKinematics::solveBodyFoot(
      NovaLeg::FrontLeft, targets.frontLeft);
  result.frontRight = NovaKinematics::solveBodyFoot(
      NovaLeg::FrontRight, targets.frontRight);
  result.rearLeft = NovaKinematics::solveBodyFoot(
      NovaLeg::RearLeft, targets.rearLeft);
  result.rearRight = NovaKinematics::solveBodyFoot(
      NovaLeg::RearRight, targets.rearRight);

  result.valid = result.frontLeft.valid &&
                 result.frontRight.valid &&
                 result.rearLeft.valid &&
                 result.rearRight.valid;

  return result;
}

void NovaMotion::printSolvedTargets(
    Stream& out,
    const NovaFootTargets& targets) {
  const NovaJointSet solved = solveTargets(targets);

  NovaKinematics::printSolution(
      out, NovaLeg::FrontLeft, targets.frontLeft, solved.frontLeft);
  NovaKinematics::printSolution(
      out, NovaLeg::FrontRight, targets.frontRight, solved.frontRight);
  NovaKinematics::printSolution(
      out, NovaLeg::RearLeft, targets.rearLeft, solved.rearLeft);
  NovaKinematics::printSolution(
      out, NovaLeg::RearRight, targets.rearRight, solved.rearRight);

  out.printf("Frame IK: %s\n", solved.valid ? "VALID" : "INVALID");
}
