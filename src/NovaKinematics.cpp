#include "NovaKinematics.h"

#include <math.h>
#include "NovaGeometry.h"

namespace {

constexpr float kRadToDeg = 180.0f / PI;
constexpr float kReachTolerance = 0.0001f;

bool isRightLeg(NovaLeg leg) {
  return leg == NovaLeg::FrontRight || leg == NovaLeg::RearRight;
}

bool isFrontLeg(NovaLeg leg) {
  return leg == NovaLeg::FrontLeft || leg == NovaLeg::FrontRight;
}

float clampUnit(float value) {
  if (value > 1.0f) return 1.0f;
  if (value < -1.0f) return -1.0f;
  return value;
}

}  // namespace

NovaPoint3 NovaKinematics::bodyToLegLocal(
    NovaLeg leg,
    const NovaPoint3& bodyPoint) {
  const float hipX = isFrontLeg(leg)
                         ? NOVA_BODY_LENGTH_M * 0.5f
                         : -NOVA_BODY_LENGTH_M * 0.5f;

  const float hipZ = isRightLeg(leg)
                         ? NOVA_BODY_WIDTH_M * 0.5f
                         : -NOVA_BODY_WIDTH_M * 0.5f;

  const float dx = bodyPoint.x - hipX;
  const float dy = bodyPoint.y;
  const float dz = bodyPoint.z - hipZ;

  // Right leg frames are rotated +90 deg around Y; left legs -90 deg.
  if (isRightLeg(leg)) {
    return NovaPoint3{-dz, dy, dx};
  }

  return NovaPoint3{dz, dy, -dx};
}

NovaJointAngles NovaKinematics::solveLegLocal(
    NovaLeg leg,
    const NovaPoint3& localPoint) {
  const float x = localPoint.x;
  const float y = localPoint.y;
  const float z = localPoint.z;

  const float l1 = NOVA_HIP_LINK_M;
  const float l2 = NOVA_UPPER_LEG_M;
  const float l3 = NOVA_LOWER_LEG_M;

  const float dRaw =
      ((x * x) + (y * y) + (z * z) -
       (l1 * l1) - (l2 * l2) - (l3 * l3)) /
      (2.0f * l2 * l3);

  const float shoulderPlaneRaw =
      (x * x) + (y * y) - (l1 * l1);

  if (dRaw > (1.0f + kReachTolerance) ||
      dRaw < (-1.0f - kReachTolerance) ||
      shoulderPlaneRaw < -kReachTolerance) {
    return NovaJointAngles{0.0f, 0.0f, 0.0f, false};
  }

  const float d = clampUnit(dRaw);
  const float shoulderPlane = max(0.0f, shoulderPlaneRaw);
  const float kneeRoot = sqrtf(max(0.0f, 1.0f - (d * d)));

  const float knee = isRightLeg(leg)
                         ? atan2f(kneeRoot, d)
                         : atan2f(-kneeRoot, d);

  const float shoulder =
      atan2f(z, sqrtf(shoulderPlane)) -
      atan2f(l3 * sinf(knee), l2 + (l3 * cosf(knee)));

  const float hip =
      atan2f(y, x) +
      atan2f(sqrtf(shoulderPlane), -l1);

  if (!isfinite(hip) || !isfinite(shoulder) || !isfinite(knee)) {
    return NovaJointAngles{0.0f, 0.0f, 0.0f, false};
  }

  return NovaJointAngles{hip, shoulder, knee, true};
}

NovaJointAngles NovaKinematics::solveBodyFoot(
    NovaLeg leg,
    const NovaPoint3& bodyPoint) {
  return solveLegLocal(leg, bodyToLegLocal(leg, bodyPoint));
}

NovaPoint3 NovaKinematics::neutralFootBody(NovaLeg leg) {
  const bool front = isFrontLeg(leg);
  const bool right = isRightLeg(leg);

  const float x = front
      ? (NOVA_BODY_LENGTH_M * 0.5f) + NOVA_STAND_FRONT_X_OFFSET_M
      : (-NOVA_BODY_LENGTH_M * 0.5f) + NOVA_STAND_REAR_X_OFFSET_M;

  const float zMagnitude =
      (NOVA_BODY_WIDTH_M * 0.5f) + NOVA_HIP_LINK_M;

  return NovaPoint3{
      x,
      -NOVA_STAND_HEIGHT_M,
      right ? zMagnitude : -zMagnitude};
}

NovaJointSet NovaKinematics::solveNeutralStance() {
  NovaJointSet set;

  set.frontLeft = solveBodyFoot(
      NovaLeg::FrontLeft, neutralFootBody(NovaLeg::FrontLeft));
  set.frontRight = solveBodyFoot(
      NovaLeg::FrontRight, neutralFootBody(NovaLeg::FrontRight));
  set.rearLeft = solveBodyFoot(
      NovaLeg::RearLeft, neutralFootBody(NovaLeg::RearLeft));
  set.rearRight = solveBodyFoot(
      NovaLeg::RearRight, neutralFootBody(NovaLeg::RearRight));

  set.valid = set.frontLeft.valid &&
              set.frontRight.valid &&
              set.rearLeft.valid &&
              set.rearRight.valid;

  return set;
}

void NovaKinematics::printGeometry(Stream& out) {
  out.println();
  out.println("Nova reference geometry:");
  out.printf("  Hip link       : %.1f mm\n", NOVA_HIP_LINK_M * 1000.0f);
  out.printf("  Upper leg      : %.1f mm\n", NOVA_UPPER_LEG_M * 1000.0f);
  out.printf("  Lower leg      : %.1f mm\n", NOVA_LOWER_LEG_M * 1000.0f);
  out.printf("  Hip length span: %.1f mm\n", NOVA_BODY_LENGTH_M * 1000.0f);
  out.printf("  Hip width span : %.1f mm\n", NOVA_BODY_WIDTH_M * 1000.0f);
  out.printf("  Stand height   : %.1f mm\n", NOVA_STAND_HEIGHT_M * 1000.0f);
  out.printf("  Physically confirmed: %s\n",
             NOVA_GEOMETRY_CONFIRMED ? "YES" : "NO - reference values only");
  out.println();
}

void NovaKinematics::printSolution(
    Stream& out,
    NovaLeg leg,
    const NovaPoint3& bodyPoint,
    const NovaJointAngles& angles) {
  out.printf("[%s] foot XYZ = %.1f, %.1f, %.1f mm\n",
             novaLegName(leg),
             bodyPoint.x * 1000.0f,
             bodyPoint.y * 1000.0f,
             bodyPoint.z * 1000.0f);

  if (!angles.valid) {
    out.printf("[%s] IK = UNREACHABLE\n", novaLegName(leg));
    return;
  }

  out.printf("[%s] IK deg: hip=%+.2f shoulder=%+.2f knee=%+.2f\n",
             novaLegName(leg),
             angles.hipRad * kRadToDeg,
             angles.shoulderRad * kRadToDeg,
             angles.kneeRad * kRadToDeg);
}
