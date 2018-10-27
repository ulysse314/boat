#include "ArduinoError.h"

#include "ArduinoController.h"

ArduinoError::ArduinoError(Code code, const char *message) :
    Error(message),
    _code(code) {
}

ArduinoError::~ArduinoError() {
}

Error::Level ArduinoError::getLevel() const {
  switch(_code) {
    case CodeNone:
    case CodeSetValueWithWrongType:
    case CodeGetValueWithWrongType:
    case CodeComputeTimeCritical:
    case CodeMemoryDifference1k:
    case CodeVeryLowMemory:
    case CodeArduinoCodeUnknown:
    case CodeGPSValueIndex:
    case CodeGPSCodeUnknown:
    case CodeAddingValueWithSameName:
    case CodePWMDriverNotAvailable:
      return Error::Level::Critical;
    case CodeComputeTimeWarning:
    case CodeMemoryDifference500:
    case CodeLowMemory:
      return Error::Level::Warning;
    case CodeComputeTimeInfo:
    case CodeMemoryDifference100:
    case CodeInfoMemory:
      return Error::Level::Info;
  };
  ArduinoController::addArduinoError(ArduinoError::CodeArduinoCodeUnknown);
  return Error::Level::Critical;
}

bool ArduinoError::isPersistant() const {
  switch(_code) {
    case CodeNone:
    case CodeSetValueWithWrongType:
    case CodeGetValueWithWrongType:
    case CodeArduinoCodeUnknown:
    case CodeGPSValueIndex:
    case CodeGPSCodeUnknown:
    case CodeMemoryDifference1k:
    case CodeComputeTimeCritical:
    case CodeAddingValueWithSameName:
      return true;
    case CodeComputeTimeInfo:
    case CodeComputeTimeWarning:
    case CodeMemoryDifference100:
    case CodeMemoryDifference500:
    case CodeInfoMemory:
    case CodeLowMemory:
    case CodeVeryLowMemory:
    case CodePWMDriverNotAvailable:
      return false;
  };
  ArduinoController::addArduinoError(ArduinoError::CodeArduinoCodeUnknown);
  return false;
}
