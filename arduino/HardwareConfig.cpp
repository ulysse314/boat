#include "HardwareConfig.h"

#include "OneWire.h"
#include "Version.h"

#include <Arduino.h>
#include <Wire.h>

// PWM IDs
#define LEFT_MOTOR_ID        0
#define RIGHT_MOTOR_ID       1

// Arduino pins
#define ONE_WIRE_PIN          12
#define LED_PIN               13

// I2C addresses
#define PCA9685_ADDRESS       0x40
#define INA219_ADDRESS        0x41
#define ADS1115_ADDRESS       0x48

#if IS_MOUSSAILLON
const uint8_t kBalancerDallasAddress[8] = { 0x28, 0xAA, 0x73, 0x4E, 0x55, 0x14, 0x01, 0x9C };
const uint8_t kBatteryDallasAddress[8] = { 0x28, 0x66, 0x05, 0x1F, 0x03, 0x00, 0x00, 0xFB };
const uint8_t kLeftMotorDallasAddress[8] = { 0x28, 0x0C, 0x17, 0x1F, 0x03, 0x00, 0x00, 0x08 };
const uint8_t kRightMotorDallasAddress[8] = { 0x28, 0x89, 0x1A, 0x1F, 0x03, 0x00, 0x00, 0xE5 };
#elif IS_TELEMAQUE
const uint8_t kBalancerDallasAddress[8] = { 0x28, 0xAA, 0x73, 0x4E, 0x55, 0x14, 0x01, 0x9C };
const uint8_t kBatteryDallasAddress[8] = { 0x28, 0x58, 0xDB, 0x1E, 0x03, 0x00, 0x00, 0x76 };
const uint8_t kLeftMotorDallasAddress[8] = { 0x28, 0xC3, 0x10, 0x1F, 0x03, 0x00, 0x00, 0xE2 };
const uint8_t kRightMotorDallasAddress[8] = { 0x28, 0xAB, 0xDD, 0x1E, 0x03, 0x00, 0x00, 0xC1 };
#else
#error *** No boat defined ***
#endif

HardwareConfig::HardwareConfig() {
}

HardwareConfig::~HardwareConfig() {
}

void HardwareConfig::begin() {
  _i2c = &Wire;
  _i2c->begin();
  _oneWire = new OneWire(ONE_WIRE_PIN);
}

const uint8_t *HardwareConfig::getLeftMotorDallasAddress() const {
  return kLeftMotorDallasAddress;
}

const uint8_t *HardwareConfig::getRightMotorDallasAddress() const {
  return kRightMotorDallasAddress;
}

const uint8_t *HardwareConfig::getBatteryDallasAddress() const {
  return kBatteryDallasAddress;
}

const uint8_t *HardwareConfig::getBalancerDallasAddress() const {
  return kBalancerDallasAddress;
}

uint8_t HardwareConfig::getPCA9685Address() const {
  return PCA9685_ADDRESS;
}

uint8_t HardwareConfig::getINA219Address() const {
  return INA219_ADDRESS;
}

uint8_t HardwareConfig::getADS1115Address() const {
  return ADS1115_ADDRESS;
}

uint8_t HardwareConfig::getLeftMotorPWMID() const {
  return LEFT_MOTOR_ID;
}

uint8_t HardwareConfig::getRightMotorPWMID() const {
  return RIGHT_MOTOR_ID;
}
