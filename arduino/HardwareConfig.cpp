#include "HardwareConfig.h"

#include "OneWire.h"
#include "Version.h"

#include <Arduino.h>
#include <Wire.h>

#define PI_SERIAL_SPEED           115200

#define GPS_SERIAL_LOW_SPEED      9600
#define GPS_SERIAL_HIGH_SPEED     115200

// PWM IDs
#define LEFT_MOTOR_ID             0
#define RIGHT_MOTOR_ID            1

// Arduino pins
#define ONE_WIRE_PIN              12
#define LED_PIN                   13

// I2C addresses
#define BNO055_ADDRESS            0x28
#define PCA9685_ADDRESS           0x40
#define INA219_ADDRESS            0x41
#define ADS1115_ADDRESS           0x48
#define BME680_ADDRESS            0x76

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

HardwareConfig::HardwareConfig() :
    _i2c(&Wire),
    _oneWire(new OneWire(ONE_WIRE_PIN)),
    _piSerial(&Serial),
    _gpsSerial(&Serial1) {
}

HardwareConfig::~HardwareConfig() {
  delete(_oneWire);
}

void HardwareConfig::begin() {
  pinMode(ledPin(), OUTPUT);
  _i2c->begin();
  _piSerial->begin(PI_SERIAL_SPEED);
  _gpsSerial->begin(GPS_SERIAL_LOW_SPEED);
}

Stream *HardwareConfig::piSerial() const {
  return _piSerial;
};

Stream *HardwareConfig::gpsSerial() const {
  return _gpsSerial;
}

void HardwareConfig::switchGPSSerialToHighSpeed() {
  _gpsSerial->flush();
  _gpsSerial->end();
  _gpsSerial->begin(GPS_SERIAL_HIGH_SPEED);
}

const OneWire::Address HardwareConfig::leftMotorDallasAddress() const {
  return kLeftMotorDallasAddress;
}

const OneWire::Address HardwareConfig::rightMotorDallasAddress() const {
  return kRightMotorDallasAddress;
}

const OneWire::Address HardwareConfig::batteryDallasAddress() const {
  return kBatteryDallasAddress;
}

const OneWire::Address HardwareConfig::balancerDallasAddress() const {
  return kBalancerDallasAddress;
}

uint8_t HardwareConfig::pca9685Address() const {
  return PCA9685_ADDRESS;
}

uint8_t HardwareConfig::ina219Address() const {
  return INA219_ADDRESS;
}

uint8_t HardwareConfig::ads1115Address() const {
  return ADS1115_ADDRESS;
}

uint8_t HardwareConfig::bme680Address() const {
  return BME680_ADDRESS;
}

uint8_t HardwareConfig::bno055Address() const {
  return BNO055_ADDRESS;
}

uint8_t HardwareConfig::leftMotorPWMID() const {
  return LEFT_MOTOR_ID;
}

uint8_t HardwareConfig::rightMotorPWMID() const {
  return RIGHT_MOTOR_ID;
}

uint8_t HardwareConfig::ledPin() const {
  return LED_PIN;
}
