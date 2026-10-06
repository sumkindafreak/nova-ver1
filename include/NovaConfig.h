#pragma once

#include <Arduino.h>

constexpr uint32_t NOVA_SERIAL_BAUD = 115200;

// I2C bus for PCA9685, IMU and front ToF sensor.
constexpr uint8_t NOVA_I2C_SDA_PIN = 8;
constexpr uint8_t NOVA_I2C_SCL_PIN = 9;
constexpr uint32_t NOVA_I2C_CLOCK_HZ = 400000;

// PCA9685.
constexpr uint8_t NOVA_PCA9685_ADDRESS = 0x40;
constexpr float NOVA_SERVO_PWM_FREQUENCY_HZ = 50.0f;

// Four legs x three joints.
constexpr uint8_t NOVA_LEG_COUNT = 4;
constexpr uint8_t NOVA_JOINTS_PER_LEG = 3;
constexpr uint8_t NOVA_SERVO_COUNT = NOVA_LEG_COUNT * NOVA_JOINTS_PER_LEG;

// Conservative direct bench-test pulse range.
constexpr uint16_t NOVA_DEFAULT_SERVO_MIN_US = 1000;
constexpr uint16_t NOVA_DEFAULT_SERVO_MAX_US = 2000;
constexpr float NOVA_DEFAULT_NEUTRAL_DEG = 90.0f;

constexpr uint8_t NOVA_FIRST_SPARE_CHANNEL = 12;

// Nominal 50 Hz motion maths period.
constexpr uint32_t NOVA_CONTROL_PERIOD_MS = 20;

// SAFETY GATE:
// Kinematics/gait calculations are active, but automatic IK-to-servo movement
// stays locked until the physical geometry and all 12 joints are calibrated.
constexpr bool NOVA_KINEMATIC_SERVO_OUTPUT_ENABLED = false;
