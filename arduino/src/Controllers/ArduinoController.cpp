#include "ArduinoController.h"

#include "../Main/HardwareConfig.h"
#include "../Main/PiLink.h"
#include "../Main/Version.h"
#include "../Sensors/DallasSensor.h"

#include <Adafruit_SleepyDog.h>
#include <Arduino.h>
#include <MemoryFree.h>
#include <OneWire.h>
#include <Wire.h>

static ArduinoController *sharedArduinoController = NULL;

#define WATCHDOG_ENABLE                  0
#define WATCHDOG_BEGIN_DELAY             10000
#define WATCHDOG_LOOP_DELAY              10000

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
    _cylcleCounter(0),
    _shouldRestart(false),
    _started(Value::Type::Boolean, "stt"),
    _loopCounter(Value::Type::Integer, "lpcnt"),
    _longestLoopDuration(Value::Type::Integer, "lpdrt"),
    _cycleDuration(Value::Type::Integer, "ccldrt"),
    _exportDuration(Value::Type::Integer, "expdrt"),
    _ramFree(Value::Type::Integer, "rf"),
    _ramFreeDifference(Value::Type::Integer, "rfd"),
    _millis(Value::Type::Integer, "mlls"),
    _timestamp(Value::Type::Integer, "tst"),
    _compileDate(Value::Type::String, "cmp"),
    _arduinoVersion(Value::Type::Integer, "vrs"),
    _debugInfo(Value::Type::String, "dbg"),
    _lastRestartCause(Value::Type::Integer, "rst") {
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
  addValue(&_loopCounter);
  addValue(&_longestLoopDuration);
  addValue(&_cycleDuration);
  addValue(&_exportDuration);
  addValue(&_ramFree);
  addValue(&_ramFreeDifference);
  addValue(&_millis);
  addValue(&_timestamp);
  addValue(&_compileDate);
  addValue(&_arduinoVersion);
  addValue(&_debugInfo);
  addValue(&_lastRestartCause);
  size_t currentFreeRAM = freeMemory();
  _infoFreeRAM = currentFreeRAM * 0.6;
  _warningFreeRAM = currentFreeRAM * 0.4;
  _criticalFreeRAM = currentFreeRAM * 0.2;
  _lastRestartCause.setInteger(Watchdog.resetCause());
#if WATCHDOG_ENABLE
  Watchdog.enable(WATCHDOG_BEGIN_DELAY);
#endif
}

void ArduinoController::sensorsHasBeenUpdated() {
  ++_cylcleCounter;
  _millis.setInteger(millis());
  unsigned long currentMillis = millis();
  unsigned long difference = currentMillis - _lastMillis;
  if (difference > 1000) {
    unsigned long addedSeconds = difference / 1000;
    _timestamp.setInteger(addedSeconds + _timestamp.valueAsInteger());
    _lastMillis += addedSeconds * 1000;
  }
  size_t currentFreeRAM = freeMemory();
  _ramFreeDifference.setInteger(currentFreeRAM - _ramFree.valueAsInteger());
  _ramFree.setInteger(currentFreeRAM);
  if (_cylcleCounter < 5 && !_started.valueAsBoolean()) {
    addError(new ArduinoError(ArduinoError::CodeNotStarted));
    return;
  }
  _started.setBoolean(true);
  if (_ramFreeDifference.valueAsInteger() >= 1000) {
    addError(new ArduinoError(ArduinoError::CodeRAMDifference1k));
  } else if (_ramFreeDifference.valueAsInteger() >= 500) {
    addError(new ArduinoError(ArduinoError::CodeRAMDifference500));
  } else if (_ramFreeDifference.valueAsInteger() >= 100) {
    addError(new ArduinoError(ArduinoError::CodeRAMDifference100));
  }
  if (currentFreeRAM <= _criticalFreeRAM) {
    addError(new ArduinoError(ArduinoError::CodeCriticalRAM));
  } else if (currentFreeRAM <= _warningFreeRAM) {
    addError(new ArduinoError(ArduinoError::CodeWarningRAM));
  } else if (currentFreeRAM <= _infoFreeRAM) {
    addError(new ArduinoError(ArduinoError::CodeInfoRAM));
  }
  if (_exportDuration.valueAsInteger() < 200) {
  } else if (_exportDuration.valueAsInteger() < 500) {
    addError(new ArduinoError(ArduinoError::CodeExportDurationInfo));
  } else if (_exportDuration.valueAsInteger() > 750) {
    addError(new ArduinoError(ArduinoError::CodeExportDurationWarning));
  } else {
    addError(new ArduinoError(ArduinoError::CodeExportDurationCritical));
  }
  if (_loopCounter.valueAsInteger() < 1000) {
    addError(new ArduinoError(ArduinoError::CodeMainLoopCounterLowCritical));
  } else if (_loopCounter.valueAsInteger() < 2500) {
    addError(new ArduinoError(ArduinoError::CodeMainLoopCounterLowWarning));
  } else if (_loopCounter.valueAsInteger() < 5000) {
    addError(new ArduinoError(ArduinoError::CodeMainLoopCounterLowInfo));
  }
  if (PiLink::getSharedPiLink()->hasTimedOut()) {
    addError(new ArduinoError(ArduinoError::CodePiLinkConnectionTimeOut));
  }
  if (!_shouldRestart) {
#if WATCHDOG_ENABLE
    Watchdog.enable(WATCHDOG_LOOP_DELAY);
#endif
  }
}

char hexFromByte(byte value) {
  return (value > 9) ? (value - 10 + 'A') : (value + '0');
}

void ArduinoController::setCommand(const char *command) {
  String result;
  if (strcmp(command, "dallasscan") == 0) {
    OneWire *oneWire = _hardwareConfig->oneWire();
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
    TwoWire *i2c = _hardwareConfig->i2c();
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

void ArduinoController::restart() {
  Watchdog.enable(100);
  _shouldRestart = true;
}
