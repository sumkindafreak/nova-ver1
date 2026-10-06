#include "NovaServoController.h"

#include <Wire.h>

#include "NovaConfig.h"

namespace {

// Channel order follows the physical leg map documented in README.md.
// Calibration values are deliberately conservative for initial bench testing.
NovaServoCalibration kServoCalibration[NOVA_SERVO_COUNT] = {
    {0, NOVA_DEFAULT_SERVO_MIN_US, NOVA_DEFAULT_SERVO_MAX_US, false, 0.0f},
    {1, NOVA_DEFAULT_SERVO_MIN_US, NOVA_DEFAULT_SERVO_MAX_US, false, 0.0f},
    {2, NOVA_DEFAULT_SERVO_MIN_US, NOVA_DEFAULT_SERVO_MAX_US, false, 0.0f},

    {3, NOVA_DEFAULT_SERVO_MIN_US, NOVA_DEFAULT_SERVO_MAX_US, false, 0.0f},
    {4, NOVA_DEFAULT_SERVO_MIN_US, NOVA_DEFAULT_SERVO_MAX_US, false, 0.0f},
    {5, NOVA_DEFAULT_SERVO_MIN_US, NOVA_DEFAULT_SERVO_MAX_US, false, 0.0f},

    {6, NOVA_DEFAULT_SERVO_MIN_US, NOVA_DEFAULT_SERVO_MAX_US, false, 0.0f},
    {7, NOVA_DEFAULT_SERVO_MIN_US, NOVA_DEFAULT_SERVO_MAX_US, false, 0.0f},
    {8, NOVA_DEFAULT_SERVO_MIN_US, NOVA_DEFAULT_SERVO_MAX_US, false, 0.0f},

    {9, NOVA_DEFAULT_SERVO_MIN_US, NOVA_DEFAULT_SERVO_MAX_US, false, 0.0f},
    {10, NOVA_DEFAULT_SERVO_MIN_US, NOVA_DEFAULT_SERVO_MAX_US, false, 0.0f},
    {11, NOVA_DEFAULT_SERVO_MIN_US, NOVA_DEFAULT_SERVO_MAX_US, false, 0.0f},
};

constexpr uint8_t kLegServoIndex[NOVA_LEG_COUNT][NOVA_JOINTS_PER_LEG] = {
    {0, 1, 2},    // Front Left
    {3, 4, 5},    // Front Right
    {6, 7, 8},    // Rear Left
    {9, 10, 11},  // Rear Right
};

}  // namespace

NovaServoController::NovaServoController()
    : pwm_(NOVA_PCA9685_ADDRESS), energized_(false) {}

bool NovaServoController::begin() {
  Serial.printf("[SERVO] Starting PCA9685 at I2C address 0x%02X\n",
                NOVA_PCA9685_ADDRESS);

  if (!pwm_.begin()) {
    Serial.println("[SERVO] ERROR: PCA9685 did not initialise.");
    return false;
  }

  // Use the PCA9685 datasheet nominal internal oscillator until the actual
  // board is measured/calibrated.
  pwm_.setOscillatorFrequency(25000000);
  pwm_.setPWMFreq(NOVA_SERVO_PWM_FREQUENCY_HZ);
  delay(10);

  // Never move the robot merely because the controller booted.
  disableAll();

  Serial.printf("[SERVO] PCA9685 ready at %.1f Hz. Outputs are OFF.\n",
                NOVA_SERVO_PWM_FREQUENCY_HZ);
  return true;
}

void NovaServoController::disableAll() {
  for (uint8_t channel = 0; channel < 16; ++channel) {
    // PCA9685 full-off bit.
    pwm_.setPWM(channel, 0, 4096);
  }

  energized_ = false;
  Serial.println("[SERVO] All 16 PWM outputs disabled.");
}

bool NovaServoController::setServoAngle(uint8_t servoIndex, float angleDeg) {
  if (servoIndex >= NOVA_SERVO_COUNT) {
    Serial.printf("[SERVO] ERROR: servo index %u is outside 0-%u.\n",
                  servoIndex,
                  NOVA_SERVO_COUNT - 1);
    return false;
  }

  const NovaServoCalibration& calibration = kServoCalibration[servoIndex];
  const uint16_t ticks = angleToTicks(calibration, angleDeg);

  pwm_.setPWM(calibration.channel, 0, ticks);
  energized_ = true;

  Serial.printf("[SERVO] S%u / CH%u -> %.1f deg (%u ticks)\n",
                servoIndex,
                calibration.channel,
                angleDeg,
                ticks);
  return true;
}

bool NovaServoController::setLegAngles(NovaLeg leg,
                                       float hipDeg,
                                       float upperDeg,
                                       float lowerDeg) {
  const uint8_t legIndex = static_cast<uint8_t>(leg);

  if (legIndex >= NOVA_LEG_COUNT) {
    Serial.println("[SERVO] ERROR: invalid leg.");
    return false;
  }

  const bool hipOk =
      setServoAngle(kLegServoIndex[legIndex][0], hipDeg);
  const bool upperOk =
      setServoAngle(kLegServoIndex[legIndex][1], upperDeg);
  const bool lowerOk =
      setServoAngle(kLegServoIndex[legIndex][2], lowerDeg);

  return hipOk && upperOk && lowerOk;
}

void NovaServoController::neutralAll() {
  Serial.println("[SERVO] Moving all configured joints to neutral.");
  for (uint8_t servoIndex = 0; servoIndex < NOVA_SERVO_COUNT; ++servoIndex) {
    setServoAngle(servoIndex, NOVA_DEFAULT_NEUTRAL_DEG);
    delay(30);
  }
}

bool NovaServoController::isEnergized() const {
  return energized_;
}

void NovaServoController::printMap(Stream& out) const {
  out.println();
  out.println("Servo map:");
  out.println("  Front Left : S0/CH0 hip, S1/CH1 upper, S2/CH2 lower");
  out.println("  Front Right: S3/CH3 hip, S4/CH4 upper, S5/CH5 lower");
  out.println("  Rear Left  : S6/CH6 hip, S7/CH7 upper, S8/CH8 lower");
  out.println("  Rear Right : S9/CH9 hip, S10/CH10 upper, S11/CH11 lower");
  out.println("  CH12-CH15  : spare");
  out.println();
}

uint16_t NovaServoController::angleToTicks(
    const NovaServoCalibration& calibration,
    float requestedAngleDeg) const {
  float angleDeg = constrain(requestedAngleDeg, 0.0f, 180.0f);

  if (calibration.reversed) {
    angleDeg = 180.0f - angleDeg;
  }

  angleDeg += calibration.offsetDeg;
  angleDeg = constrain(angleDeg, 0.0f, 180.0f);

  const float pulseUs =
      calibration.minPulseUs +
      ((calibration.maxPulseUs - calibration.minPulseUs) * (angleDeg / 180.0f));

  // One 50 Hz PWM frame is 20,000 us and the PCA9685 divides it into 4096 ticks.
  const float frameUs = 1000000.0f / NOVA_SERVO_PWM_FREQUENCY_HZ;
  const float tickUs = frameUs / 4096.0f;

  return static_cast<uint16_t>(pulseUs / tickUs);
}
