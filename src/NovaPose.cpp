#include "NovaPose.h"

#include "NovaGeometry.h"
#include "NovaKinematics.h"

namespace {

NovaPoint3 footAtHeight(NovaLeg leg, float bodyHeightM) {
  NovaPoint3 foot = NovaKinematics::neutralFootBody(leg);
  foot.y = -bodyHeightM;
  return foot;
}

}  // namespace

NovaFootTargets NovaPose::stance(float bodyHeightM) {
  NovaFootTargets targets;
  targets.frontLeft = footAtHeight(NovaLeg::FrontLeft, bodyHeightM);
  targets.frontRight = footAtHeight(NovaLeg::FrontRight, bodyHeightM);
  targets.rearLeft = footAtHeight(NovaLeg::RearLeft, bodyHeightM);
  targets.rearRight = footAtHeight(NovaLeg::RearRight, bodyHeightM);
  return targets;
}

NovaFootTargets NovaPose::stand() {
  return stance(NOVA_STAND_HEIGHT_M);
}

NovaFootTargets NovaPose::crouch() {
  return stance(0.1200f);
}
