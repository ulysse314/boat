#include "ArduinoController.h"

#include "DallasSensor.h"
#include "HardwareConfig.h"
#include "MemoryFree.h"
#include "PiLink.h"
#include "Version.h"

#include <Arduino.h>
#include <OneWire.h>
#include <Wire.h>

static ArduinoController *sharedArduinoController = NULL;

// static
ArduinoController *ArduinoController::generateController(HardwareConfig *hardwareConfig) {
  if (!sharedArduinoController) {
    sharedArduinoController = new ArduinoController(hardwareConfig);
  }
  return sharedArduinoController;
}

// static
ArduinoController *ArduinoController::sharedController() {
  return sharedArduinoController;
}

ArduinoController::ArduinoController(HardwareConfig *hardwareConfig) :
    _hardwareConfig(hardwareConfig),
    _infoFreeRAM(0),
    _warningFreeRAM(0),
    _criticalFreeRAM(0),
    _lastMillis(0),
    _loopCount(0),
    _started(Value::Type::Boolean, "stt"),
    _cycleCount(Value::Type::Integer, "cyc"),
    _loopDuration(Value::Type::Integer, "ld"),
    _computeTime(Value::Type::Integer, "ct"),
    _ramFree(Value::Type::Integer, "rf"),
    _ramFreeDifference(Value::Type::Integer, "rfd"),
    _millis(Value::Type::Integer, "mil"),
    _timestamp(Value::Type::Integer, "tst"),
    _compileDate(Value::Type::String, "cmp"),
    _arduinoVersion(Value::Type::Integer, "ver"),
    _debugInfo(Value::Type::String, "debug") {
  String compileDateString;
  compileDateString.concat(__TIME__);
  compileDateString.concat(" ");
  compileDateString.concat(__DATE__);
  _compileDate.setString(compileDateString.c_str());
  _arduinoVersion.setInteger(ArduinoVersion);
}

ArduinoController::~ArduinoController() {
}

void ArduinoController::begin() {
  addValue(&_started);
  addValue(&_cycleCount);
  addValue(&_loopDuration);
  addValue(&_computeTime);
  addValue(&_ramFree);
  addValue(&_ramFreeDifference);
  addValue(&_millis);
  addValue(&_timestamp);
  addValue(&_compileDate);
  addValue(&_arduinoVersion);
  addValue(&_debugInfo);
  size_t currentFreeRAM = freeMemory();
  _infoFreeRAM = currentFreeRAM * 0.6;
  _warningFreeRAM = currentFreeRAM * 0.4;
  _criticalFreeRAM = currentFreeRAM * 0.2;
}

void ArduinoController::sensorsHasBeenUpdated() {
  ++_loopCount;
  _millis.setInteger(millis());
  unsigned long currentMillis = millis();
  unsigned long difference = currentMillis - _lastMillis;
  if (difference > 1000) {
    unsigned long addedSeconds = difference / 1000;
    _timestamp.setInteger(addedSeconds + _timestamp.getInteger());
    _lastMillis += addedSeconds * 1000;
  }
  size_t currentFreeRAM = freeMemory();
  _ramFreeDifference.setInteger(currentFreeRAM - _ramFree.getInteger());
  _ramFree.setInteger(currentFreeRAM);
  if (_loopCount < 5 && !_started.getBoolean()) {
    addError(new ArduinoError(ArduinoError::CodeNotStarted));
    return;
  }
  _started.setBoolean(true);
  if (_ramFreeDifference.getInteger() >= 1000) {
    addError(new ArduinoError(ArduinoError::CodeRAMDifference1k));
  } else if (_ramFreeDifference.getInteger() >= 500) {
    addError(new ArduinoError(ArduinoError::CodeRAMDifference500));
  } else if (_ramFreeDifference.getInteger() >= 100) {
    addError(new ArduinoError(ArduinoError::CodeRAMDifference100));
  }
  if (currentFreeRAM <= _criticalFreeRAM) {
    addError(new ArduinoError(ArduinoError::CodeCriticalRAM));
  } else if (currentFreeRAM <= _warningFreeRAM) {
    addError(new ArduinoError(ArduinoError::CodeWarningRAM));
  } else if (currentFreeRAM <= _infoFreeRAM) {
    addError(new ArduinoError(ArduinoError::CodeInfoRAM));
  }
  if (_computeTime.getInteger() < 200) {
  } else if (_computeTime.getInteger() < 500) {
    addError(new ArduinoError(ArduinoError::CodeComputeTimeInfo));
  } else if (_computeTime.getInteger() > 750) {
    addError(new ArduinoError(ArduinoError::CodeComputeTimeWarning));
  } else {
    addError(new ArduinoError(ArduinoError::CodeComputeTimeCritical));
  }
  if (_cycleCount.getInteger() < 1000) {
    addError(new ArduinoError(ArduinoError::CodeMainLoopCounterLowCritical));
  } else if (_cycleCount.getInteger() < 2500) {
    addError(new ArduinoError(ArduinoError::CodeMainLoopCounterLowWarning));
  } else if (_cycleCount.getInteger() < 5000) {
    addError(new ArduinoError(ArduinoError::CodeMainLoopCounterLowInfo));
  }
  if (PiLink::getSharedPiLink()->hasTimedOut()) {
    addError(new ArduinoError(ArduinoError::CodePiLinkConnectionTimeOut));
  }
}

char hexFromByte(byte value) {
  return (value > 9) ? (value - 10 + 'A') : (value + '0');
}

void ArduinoController::setCommand(const char *command) {
  String result;
  if (strcmp(command, "dallasscan") == 0) {
    OneWire *oneWire = _hardwareConfig->getOneWire();
    OneWire::Address addr;
    while(oneWire->search(addr)) {
      if (result.length() > 0) {
        result = result + ", ";
      }
      DallasSensor sensor(addr, oneWire);
      sensor.readValues();
      const char *address = sensor.copyAddressString();
      result = result + address;
      free((void *)address);
      delay(1000);
      sensor.loop();
      result = result + "->";
      result = result + sensor.celsius();
    }
    oneWire->reset_search();
    result = "Dallas: " + result;
  } else if (strcmp(command, "i2cscan") == 0) {
    TwoWire *i2c = _hardwareConfig->getI2C();
    for(byte address = 1; address < 127; address++ ) {
      i2c->beginTransmission(address);
      byte error = i2c->endTransmission();
      if (error != 0) {
        continue;
      }
      if (result.length() > 0) {
        result = result + ", ";
      }
      result = result + "0x";
      result = result + hexFromByte(address >> 4);
      result = result + hexFromByte(address & 0xF);
    }
    result = "I2C: " + result;
  }
  _debugInfo.setString(result.c_str());
}
