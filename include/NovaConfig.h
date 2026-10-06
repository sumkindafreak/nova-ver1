#pragma once

#include <Arduino.h>

// -----------------------------------------------------------------------------
// Nova v1 central hardware configuration
// -----------------------------------------------------------------------------

// USB serial console.
constexpr uint32_t NOVA_SERIAL_BAUD = 115200;

// I2C bus used by the PCA9685, IMU/gyro and front ToF sensor.
// These are common ESP32-S3 DevKitC-1 choices and can be changed here if the
// exact S3 board used for Nova is wired differently.
constexpr uint8_t NOVA_I2C_SDA_PIN = 8;
constexpr uint8_t NOVA_I2C_SCL_PIN = 9;
constexpr uint32_t NOVA_I2C_CLOCK_HZ = 400000;

// PCA9685 servo controller.
constexpr uint8_t NOVA_PCA9685_ADDRESS = 0x40;
constexpr float NOVA_SERVO_PWM_FREQUENCY_HZ = 50.0f;

// Nova has four legs and three powered joints on each leg.
constexpr uint8_t NOVA_LEG_COUNT = 4;
constexpr uint8_t NOVA_JOINTS_PER_LEG = 3;
constexpr uint8_t NOVA_SERVO_COUNT = NOVA_LEG_COUNT * NOVA_JOINTS_PER_LEG;

// Conservative initial pulse limits for bench testing.
// Replace these per joint after physical calibration.
constexpr uint16_t NOVA_DEFAULT_SERVO_MIN_US = 1000;
constexpr uint16_t NOVA_DEFAULT_SERVO_MAX_US = 2000;
constexpr float NOVA_DEFAULT_NEUTRAL_DEG = 90.0f;

// The four unused PCA9685 outputs remain available for future hardware.
constexpr uint8_t NOVA_FIRST_SPARE_CHANNEL = 12;
