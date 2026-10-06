#include <Arduino.h>
#include <Wire.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "NovaConfig.h"
#include "NovaServoController.h"

NovaServoController servoController;

namespace {

String serialLine;
bool pcaReady = false;

void printBanner() {
  Serial.println();
  Serial.println("==================================================");
  Serial.println(" NOVA v1 - ESP32-S3 quadruped bench firmware");
  Serial.println(" Phase 0: electrical + servo bring-up");
  Serial.println("==================================================");
}

void printHelp() {
  Serial.println();
  Serial.println("Commands:");
  Serial.println("  help");
  Serial.println("      Show this command list.");
  Serial.println("  scan");
  Serial.println("      Scan the I2C bus.");
  Serial.println("  status");
  Serial.println("      Show controller state and servo map.");
  Serial.println("  neutral");
  Serial.println("      Move all 12 configured joints to 90 degrees.");
  Serial.println("  disable");
  Serial.println("      Immediately disable all 16 PCA9685 outputs.");
  Serial.println("  servo <index> <angle>");
  Serial.println("      Move servo index 0-11 to angle 0-180.");
  Serial.println("      Example: servo 0 90");
  Serial.println("  leg <FL|FR|RL|RR> <hip> <upper> <lower>");
  Serial.println("      Move one complete leg.");
  Serial.println("      Example: leg FL 90 80 100");
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
        Serial.print("  <- common ToF address");
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
  servoController.printMap(Serial);
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

  if (command.equalsIgnoreCase("disable")) {
    servoController.disableAll();
    return;
  }

  if (command.equalsIgnoreCase("neutral")) {
    if (!pcaReady) {
      Serial.println("[CMD] ERROR: PCA9685 is not ready.");
      return;
    }

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
  float hipAngle = 0.0f;
  float upperAngle = 0.0f;
  float lowerAngle = 0.0f;

  if (sscanf(command.c_str(),
             "leg %2s %f %f %f",
             legText,
             &hipAngle,
             &upperAngle,
             &lowerAngle) == 4) {
    if (!pcaReady) {
      Serial.println("[CMD] ERROR: PCA9685 is not ready.");
      return;
    }

    for (uint8_t i = 0; i < 2; ++i) {
      legText[i] = static_cast<char>(toupper(legText[i]));
    }

    NovaLeg leg;
    if (!parseLegName(legText, leg)) {
      Serial.println("[CMD] ERROR: leg must be FL, FR, RL or RR.");
      return;
    }

    if (hipAngle < 0.0f || hipAngle > 180.0f ||
        upperAngle < 0.0f || upperAngle > 180.0f ||
        lowerAngle < 0.0f || lowerAngle > 180.0f) {
      Serial.println("[CMD] ERROR: all leg angles must be 0-180.");
      return;
    }

    servoController.setLegAngles(leg, hipAngle, upperAngle, lowerAngle);
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

  printHelp();
  printStatus();

  Serial.println("[BOOT] Nova is ready for bench commands.");
}

void loop() {
  serviceSerial();
  delay(2);
}
