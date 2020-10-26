#include "LidController.h"

#include <I2CDevice.h>

#include "../Main/HardwareConfig.h"

LidController::LidController(HardwareConfig *hardwareConfig) :
    _i2cDevice(new I2CDevice(hardwareConfig->lidAddress(), hardwareConfig->i2c())) {
}

LidController::~LidController() {
}

void LidController::begin() {
}

bool LidController::playLight(size_t lightIndex) {
  return sendPlayCommand("PL", lightIndex);
}

bool LidController::stopLight() {
  return sendCommand("SL");
}

bool LidController::playSong(size_t songIndex) {
  return sendPlayCommand("PS", songIndex);
}

bool LidController::stopSong() {
  return sendCommand("SS");
}

bool LidController::sendPlayCommand(String command, size_t index) {
  command += String(index);
  return sendCommand(command);
}

bool LidController::sendCommand(String command) {
  size_t bufferSize = command.length();
  if (_i2cDevice->writeBuffer((const uint8_t*)command.c_str(), bufferSize, false) != bufferSize) {
    return false;
  }
  uint8_t result;
  if (_i2cDevice->readBuffer(&result, 1) != 1) {
    return false;
  }
  return result == '+';
}
