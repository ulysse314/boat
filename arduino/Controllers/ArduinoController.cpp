#include "ArduinoController.h"

#include <Arduino.h>

#include "MemoryFree.h"
#include "Version.h"

// static
ArduinoController *ArduinoController::getArduinoController() {
  static ArduinoController *arduinoController = new ArduinoController();
  return arduinoController;
}

void ArduinoController::addArduinoError(ArduinoError::Code code) {
  ArduinoController *arduinoController = ArduinoController::getArduinoController();
  Error *error = new ArduinoError(code, NULL);
  arduinoController->addError(error);
}

ArduinoController::ArduinoController() :
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
    _arduinoVersion(Value::Type::Integer, "ver") {
  String compileDateString;
  compileDateString.concat(__TIME__);
  compileDateString.concat(" ");
  compileDateString.concat(__DATE__);
  _compileDate.setString(compileDateString.c_str());
  _arduinoVersion.setInteger(ArduinoVersion);
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
  size_t currentFreeMemory = freeMemory();
  _infoFreeMemory = currentFreeMemory * 0.6;
  _lowFreeMemory = currentFreeMemory * 0.4;
  _veryLowFreeMemory = currentFreeMemory * 0.2;
}

const Value *ArduinoController::getValueAtIndex(int index) const {
  switch((ValueIndex)index) {
    case StartedIndex:
      return &_started;
    case CycleCountIndex:
      return &_cycleCount;
    case LoopDurationIndex:
      return &_loopDuration;
    case ComputeTimeIndex:
      return &_computeTime;
    case RAMFreeIndex:
      return &_ramFree;
    case RAMFreeDifferenceIndex:
      return &_ramFreeDifference;
    case MillisIndex:
      return &_millis;
    case TimestampIndex:
      return &_timestamp;
    case CompileDateIndex:
      return &_compileDate;
    case ArduinoVersionIndex:
      return &_arduinoVersion;
    case MaxIndex:
      break;
  }
  ArduinoController::addArduinoError(ArduinoError::CodeArduinoValueIndex);
  return NULL;
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
}
