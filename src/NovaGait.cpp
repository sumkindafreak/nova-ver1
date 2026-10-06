#include "NovaGait.h"

#include "NovaGeometry.h"
#include "NovaMotion.h"
#include "NovaPose.h"

namespace {

float clamp01(float value) {
  return constrain(value, 0.0f, 1.0f);
}

float triangle01(float progress) {
  if (progress <= 0.5f) {
    return progress * 2.0f;
  }
  return (1.0f - progress) * 2.0f;
}

NovaPoint3* selectFoot(NovaFootTargets& feet, NovaLeg leg) {
  switch (leg) {
    case NovaLeg::FrontLeft: return &feet.frontLeft;
    case NovaLeg::FrontRight: return &feet.frontRight;
    case NovaLeg::RearLeft: return &feet.rearLeft;
    case NovaLeg::RearRight: return &feet.rearRight;
  }
  return &feet.frontLeft;
}

NovaPoint3 bodyShiftForPhase(uint8_t phase) {
  switch (phase) {
    case 0:
    case 1:
      return NovaPoint3{
          NOVA_CRAWL_FORWARD_SHIFT_M, 0.0f, -NOVA_CRAWL_SIDE_SHIFT_M};

    case 2:
    case 3:
      return NovaPoint3{
          -NOVA_CRAWL_REAR_SHIFT_M, 0.0f, -NOVA_CRAWL_SIDE_SHIFT_M};

    case 4:
    case 5:
      return NovaPoint3{
          NOVA_CRAWL_FORWARD_SHIFT_M, 0.0f, NOVA_CRAWL_SIDE_SHIFT_M};

    case 6:
    case 7:
    default:
      return NovaPoint3{
          -NOVA_CRAWL_REAR_SHIFT_M, 0.0f, NOVA_CRAWL_SIDE_SHIFT_M};
  }
}

NovaLeg swingLegForPhase(uint8_t phase) {
  switch (phase) {
    case 1: return NovaLeg::RearRight;
    case 3: return NovaLeg::FrontRight;
    case 5: return NovaLeg::RearLeft;
    case 7:
    default: return NovaLeg::FrontLeft;
  }
}

}  // namespace

NovaGaitFrame NovaGait::sampleCrawl(uint8_t phase, float progress) {
  NovaGaitFrame frame;

  frame.phase = phase % 8;
  frame.progress = clamp01(progress);
  frame.feet = NovaPose::stand();
  frame.bodyShift = bodyShiftForPhase(frame.phase);
  frame.swingActive = (frame.phase % 2) == 1;
  frame.swingLeg = swingLegForPhase(frame.phase);

  if (!frame.swingActive) {
    return frame;
  }

  NovaPoint3* swingFoot = selectFoot(frame.feet, frame.swingLeg);

  const float halfStep = NOVA_CRAWL_STEP_LENGTH_M * 0.5f;
  swingFoot->x += (-halfStep) +
                  (NOVA_CRAWL_STEP_LENGTH_M * frame.progress);

  // +Y is up, so foot lift makes the negative standing Y less negative.
  swingFoot->y += triangle01(frame.progress) * NOVA_CRAWL_CLEARANCE_M;

  return frame;
}

void NovaGait::printFrame(Stream& out, const NovaGaitFrame& frame) {
  out.println();
  out.printf("Crawl phase %u / progress %.0f%%\n",
             frame.phase,
             frame.progress * 100.0f);

  out.printf("Body shift target: X=%+.1f mm Z=%+.1f mm\n",
             frame.bodyShift.x * 1000.0f,
             frame.bodyShift.z * 1000.0f);

  if (frame.swingActive) {
    out.printf("Swing leg: %s\n", novaLegName(frame.swingLeg));
  } else {
    out.println("Swing leg: none - body shift phase");
  }

  NovaMotion::printSolvedTargets(out, frame.feet);
  out.println();
}
