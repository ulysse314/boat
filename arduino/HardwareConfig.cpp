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
#define BNO055_ADDRESS        0x28
#define PCA9685_ADDRESS       0x70
#define INA219_ADDRESS        0x41
#define ADS1115_ADDRESS       0x48
#define BME680_ADDRESS        0x76

#if IS_MOUSSAILLON
const OneWire::Address kBalancerDallasAddress(0x28, 0xAA, 0x73, 0x4E, 0x55, 0x14, 0x01, 0x9C);
const OneWire::Address kBatteryDallasAddress(0x28, 0x66, 0x05, 0x1F, 0x03, 0x00, 0x00, 0xFB);
const OneWire::Address kLeftMotorDallasAddress(0x28, 0x0C, 0x17, 0x1F, 0x03, 0x00, 0x00, 0x08);
const OneWire::Address kRightMotorDallasAddress(0x28, 0x89, 0x1A, 0x1F, 0x03, 0x00, 0x00, 0xE5);
#elif IS_TELEMAQUE
const OneWire::Address kBalancerDallasAddress(0x28, 0xAA, 0x73, 0x4E, 0x55, 0x14, 0x01, 0x9C);
const OneWire::Address kBatteryDallasAddress(0x28, 0x58, 0xDB, 0x1E, 0x03, 0x00, 0x00, 0x76);
const OneWire::Address kLeftMotorDallasAddress(0x28, 0xC3, 0x10, 0x1F, 0x03, 0x00, 0x00, 0xE2);
const OneWire::Address kRightMotorDallasAddress(0x28, 0xAB, 0xDD, 0x1E, 0x03, 0x00, 0x00, 0xC1);
#else
#error *** No boat defined ***
#endif

HardwareConfig::HardwareConfig() {
  _i2c = &Wire;
  _oneWire = new OneWire(ONE_WIRE_PIN);
}

HardwareConfig::~HardwareConfig() {
}

void HardwareConfig::begin() {
  _i2c->begin();
}

const OneWire::Address HardwareConfig::getLeftMotorDallasAddress() const {
  return kLeftMotorDallasAddress;
}

const OneWire::Address HardwareConfig::getRightMotorDallasAddress() const {
  return kRightMotorDallasAddress;
}

const OneWire::Address HardwareConfig::getBatteryDallasAddress() const {
  return kBatteryDallasAddress;
}

const OneWire::Address HardwareConfig::getBalancerDallasAddress() const {
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

uint8_t HardwareConfig::getBME680Address() const {
  return BME680_ADDRESS;
}

uint8_t HardwareConfig::getLeftMotorPWMID() const {
  return LEFT_MOTOR_ID;
}

uint8_t HardwareConfig::getRightMotorPWMID() const {
  return RIGHT_MOTOR_ID;
}
