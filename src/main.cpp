#include <Arduino.h>
#include <Wire.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "NovaConfig.h"
#include "NovaGait.h"
#include "NovaGeometry.h"
#include "NovaKinematics.h"
#include "NovaMotion.h"
#include "NovaPose.h"
#include "NovaServoController.h"

NovaServoController servoController;

namespace {

String serialLine;
bool pcaReady = false;

void printBanner() {
  Serial.println();
  Serial.println("==================================================");
  Serial.println(" NOVA v1 - ESP32-S3 quadruped firmware");
  Serial.println(" Bench control + geometry + IK + crawl preview");
  Serial.println("==================================================");
}

void printHelp() {
  Serial.println();
  Serial.println("Bench commands:");
  Serial.println("  help");
  Serial.println("  scan");
  Serial.println("  status");
  Serial.println("  neutral");
  Serial.println("  disable");
  Serial.println("  servo <index> <angle>");
  Serial.println("  leg <FL|FR|RL|RR> <hip> <upper> <lower>");
  Serial.println();
  Serial.println("Kinematics / motion preview commands:");
  Serial.println("  geometry");
  Serial.println("  stancecalc");
  Serial.println("  ik <FL|FR|RL|RR> <x_mm> <y_mm> <z_mm>");
  Serial.println("  crawl <phase 0-7> <progress 0-100>");
  Serial.println();
}

void scanI2C() {
  Serial.printf("[I2C] Scanning SDA=%u SCL=%u...\n",
                NOVA_I2C_SDA_PIN,
                NOVA_I2C_SCL_PIN);

  uint8_t found = 0;

  for (uint8_t address = 1; address < 127; ++address) {
    Wire.beginTransmission(address);
    const uint8_t result = Wire.endTransmission();

    if (result == 0) {
      Serial.printf("[I2C] Found device at 0x%02X", address);

      if (address == NOVA_PCA9685_ADDRESS) {
        Serial.print("  <- PCA9685 expected address");
      } else if (address == 0x29) {
        Serial.print("  <- common VL53 ToF address");
      } else if (address == 0x68 || address == 0x69) {
        Serial.print("  <- common IMU address");
      }

      Serial.println();
      ++found;
    }
  }

  if (found == 0) {
    Serial.println("[I2C] No devices found.");
  } else {
    Serial.printf("[I2C] Scan complete: %u device(s).\n", found);
  }
}

bool parseLegName(const char* text, NovaLeg& leg) {
  if (strcmp(text, "FL") == 0) {
    leg = NovaLeg::FrontLeft;
    return true;
  }
  if (strcmp(text, "FR") == 0) {
    leg = NovaLeg::FrontRight;
    return true;
  }
  if (strcmp(text, "RL") == 0) {
    leg = NovaLeg::RearLeft;
    return true;
  }
  if (strcmp(text, "RR") == 0) {
    leg = NovaLeg::RearRight;
    return true;
  }
  return false;
}

void uppercaseLeg(char* legText) {
  for (uint8_t i = 0; i < 2 && legText[i] != '\0'; ++i) {
    legText[i] = static_cast<char>(toupper(legText[i]));
  }
}

void printStatus() {
  Serial.println();
  Serial.println("Nova status:");
  Serial.printf("  PCA9685: %s\n", pcaReady ? "ready" : "not ready");
  Serial.printf("  PWM outputs: %s\n",
                servoController.isEnergized() ? "energized" : "disabled");
  Serial.printf("  I2C SDA/SCL: GPIO%u / GPIO%u\n",
                NOVA_I2C_SDA_PIN,
                NOVA_I2C_SCL_PIN);
  Serial.printf("  Servo count: %u\n", NOVA_SERVO_COUNT);
  Serial.printf("  Geometry physically confirmed: %s\n",
                NOVA_GEOMETRY_CONFIRMED ? "YES" : "NO");
  Serial.printf("  IK/gait -> servo output: %s\n",
                NOVA_KINEMATIC_SERVO_OUTPUT_ENABLED ? "ENABLED" : "LOCKED");
  servoController.printMap(Serial);
}

void printNeutralStanceCalculation() {
  Serial.println();
  Serial.println("[IK] Neutral stance calculation only - servos will not move.");
  const NovaFootTargets stand = NovaPose::stand();
  NovaMotion::printSolvedTargets(Serial, stand);
  Serial.println();
}

void processCommand(String command) {
  command.trim();
  if (command.length() == 0) {
    return;
  }

  Serial.printf("[CMD] %s\n", command.c_str());

  if (command.equalsIgnoreCase("help")) {
    printHelp();
    return;
  }
  if (command.equalsIgnoreCase("scan")) {
    scanI2C();
    return;
  }
  if (command.equalsIgnoreCase("status")) {
    printStatus();
    return;
  }
  if (command.equalsIgnoreCase("geometry")) {
    NovaKinematics::printGeometry(Serial);
    return;
  }
  if (command.equalsIgnoreCase("stancecalc")) {
    printNeutralStanceCalculation();
    return;
  }
  if (command.equalsIgnoreCase("disable")) {
    servoController.disableAll();
    return;
  }

  if (command.equalsIgnoreCase("neutral")) {
    if (!pcaReady) {
      Serial.println("[CMD] ERROR: PCA9685 is not ready.");
      return;
    }
    Serial.println("[SAFETY] Direct 90-degree bench command.");
    servoController.neutralAll();
    return;
  }

  int servoIndex = -1;
  float servoAngle = 0.0f;

  if (sscanf(command.c_str(), "servo %d %f", &servoIndex, &servoAngle) == 2) {
    if (!pcaReady) {
      Serial.println("[CMD] ERROR: PCA9685 is not ready.");
      return;
    }
    if (servoIndex < 0 || servoIndex >= NOVA_SERVO_COUNT) {
      Serial.printf("[CMD] ERROR: servo index must be 0-%u.\n",
                    NOVA_SERVO_COUNT - 1);
      return;
    }
    if (servoAngle < 0.0f || servoAngle > 180.0f) {
      Serial.println("[CMD] ERROR: angle must be 0-180.");
      return;
    }
    servoController.setServoAngle(static_cast<uint8_t>(servoIndex), servoAngle);
    return;
  }

  char legText[3] = {};
  float a = 0.0f;
  float b = 0.0f;
  float c = 0.0f;

  if (sscanf(command.c_str(),
             "leg %2s %f %f %f",
             legText, &a, &b, &c) == 4) {
    if (!pcaReady) {
      Serial.println("[CMD] ERROR: PCA9685 is not ready.");
      return;
    }

    uppercaseLeg(legText);
    NovaLeg leg;
    if (!parseLegName(legText, leg)) {
      Serial.println("[CMD] ERROR: leg must be FL, FR, RL or RR.");
      return;
    }

    if (a < 0.0f || a > 180.0f ||
        b < 0.0f || b > 180.0f ||
        c < 0.0f || c > 180.0f) {
      Serial.println("[CMD] ERROR: all leg angles must be 0-180.");
      return;
    }

    servoController.setLegAngles(leg, a, b, c);
    return;
  }

  if (sscanf(command.c_str(),
             "ik %2s %f %f %f",
             legText, &a, &b, &c) == 4) {
    uppercaseLeg(legText);
    NovaLeg leg;
    if (!parseLegName(legText, leg)) {
      Serial.println("[CMD] ERROR: leg must be FL, FR, RL or RR.");
      return;
    }

    const NovaPoint3 bodyPoint{
        a / 1000.0f,
        b / 1000.0f,
        c / 1000.0f};

    const NovaJointAngles angles =
        NovaKinematics::solveBodyFoot(leg, bodyPoint);

    Serial.println("[IK] Calculation only - servos will not move.");
    NovaKinematics::printSolution(Serial, leg, bodyPoint, angles);
    return;
  }

  int phase = -1;
  float progressPercent = 0.0f;

  if (sscanf(command.c_str(),
             "crawl %d %f",
             &phase, &progressPercent) == 2) {
    if (phase < 0 || phase > 7) {
      Serial.println("[CMD] ERROR: crawl phase must be 0-7.");
      return;
    }
    if (progressPercent < 0.0f || progressPercent > 100.0f) {
      Serial.println("[CMD] ERROR: crawl progress must be 0-100.");
      return;
    }

    Serial.println("[GAIT] Preview only - servo output is safety-locked.");

    const NovaGaitFrame frame =
        NovaGait::sampleCrawl(static_cast<uint8_t>(phase),
                              progressPercent / 100.0f);

    NovaGait::printFrame(Serial, frame);
    return;
  }

  Serial.println("[CMD] Unknown command. Type 'help'.");
}

void serviceSerial() {
  while (Serial.available() > 0) {
    const char c = static_cast<char>(Serial.read());

    if (c == '\r') {
      continue;
    }

    if (c == '\n') {
      processCommand(serialLine);
      serialLine = "";
      continue;
    }

    if (serialLine.length() < 120) {
      serialLine += c;
    }
  }
}

}  // namespace

void setup() {
  Serial.begin(NOVA_SERIAL_BAUD);
  delay(1200);

  printBanner();

  Wire.begin(NOVA_I2C_SDA_PIN, NOVA_I2C_SCL_PIN);
  Wire.setClock(NOVA_I2C_CLOCK_HZ);

  Serial.printf("[I2C] Bus started at %lu Hz.\n",
                static_cast<unsigned long>(NOVA_I2C_CLOCK_HZ));

  scanI2C();

  pcaReady = servoController.begin();

  if (!pcaReady) {
    Serial.println("[BOOT] PCA9685 unavailable. Servo commands are locked out.");
  }

  NovaKinematics::printGeometry(Serial);
  printNeutralStanceCalculation();
  printHelp();
  printStatus();

  Serial.println("[BOOT] Nova is ready.");
}

void loop() {
  serviceSerial();
  delay(2);
}
