#include "ArduinoController.h"

#include <Arduino.h>

#include "DallasSensor.h"
#include "MemoryFree.h"
#include "OneWire.h"
#include "PiLink.h"
#include "Version.h"

static ArduinoController *arduinoController = NULL;

// static
ArduinoController *ArduinoController::getArduinoController() {
  return arduinoController;
}

// static
bool ArduinoController::addArduinoError(ArduinoError::Code code) {
  ArduinoController *arduinoController = ArduinoController::getArduinoController();
  Error *error = new ArduinoError(code, NULL);
  return arduinoController->addError(error);
}

// static
bool ArduinoController::removeArduinoError(ArduinoError::Code code) {
  ArduinoController *arduinoController = ArduinoController::getArduinoController();
  Error *error = new ArduinoError(code, NULL);
  return arduinoController->removeError(error);
}

ArduinoController::ArduinoController(TwoWire *ic2, OneWire *oneWire) :
    _ic2(ic2),
    _oneWire(oneWire),
    _infoFreeMemory(0),
    _lowFreeMemory(0),
    _veryLowFreeMemory(0),
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
  arduinoController = this;
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
  size_t currentFreeMemory = freeMemory();
  _infoFreeMemory = currentFreeMemory * 0.6;
  _lowFreeMemory = currentFreeMemory * 0.4;
  _veryLowFreeMemory = currentFreeMemory * 0.2;
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
  size_t currentFreeMemory = freeMemory();
  _ramFreeDifference.setInteger(currentFreeMemory - _ramFree.getInteger());
  _ramFree.setInteger(currentFreeMemory);
  if (_loopCount < 5 && !_started.getBoolean()) {
    addError(new ArduinoError(ArduinoError::CodeNotStarted));
    return;
  }
  _started.setBoolean(true);
  if (_ramFreeDifference.getInteger() >= 1000) {
    addError(new ArduinoError(ArduinoError::CodeMemoryDifference1k));
  } else if (_ramFreeDifference.getInteger() >= 500) {
    addError(new ArduinoError(ArduinoError::CodeMemoryDifference500));
  } else if (_ramFreeDifference.getInteger() >= 100) {
    addError(new ArduinoError(ArduinoError::CodeMemoryDifference100));
  }
  if (currentFreeMemory <= _veryLowFreeMemory) {
    addError(new ArduinoError(ArduinoError::CodeVeryLowMemory));
  } else if (currentFreeMemory <= _lowFreeMemory) {
    addError(new ArduinoError(ArduinoError::CodeLowMemory));
  } else if (currentFreeMemory <= _infoFreeMemory) {
    addError(new ArduinoError(ArduinoError::CodeInfoMemory));
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
    addError(new ArduinoError(ArduinoError::CodeLoopCycleLowCritical));
  } else if (_cycleCount.getInteger() < 2500) {
    addError(new ArduinoError(ArduinoError::CodeLoopCycleLowWarning));
  } else if (_cycleCount.getInteger() < 5000) {
    addError(new ArduinoError(ArduinoError::CodeLoopCycleLowInfo));
  }
  if (PiLink::getPiLink()->hasTimedOut()) {
    addError(new ArduinoError(ArduinoError::CodePiLinkConnectionTimeOut));
  }
}

void ArduinoController::setCommand(const char *command) {
  String result;
  if (strcmp(command, "dallasscan") == 0) {
    byte addr[8];
    while(_oneWire->search(addr)) {
      if (OneWire::crc8(addr, 7) != addr[7]) {
        continue;
      }
      if (result.length() > 0) {
        result = result + "/";
      }
      DallasSensor sensor(addr, _oneWire);
      sensor.readValues();
      const char *address = sensor.copyAddressString();
      result = result + address;
      free((void *)address);
      delay(1000);
      sensor.loop();
      result = result + "->";
      result = result + sensor.celsius();
    }
    _oneWire->reset_search();
    result = "Dallas " + result;
    _debugInfo.setString(result.c_str());
  } else if (strcmp(command, "i2cscan") == 0) {
  }
}
