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
    case CodeRAMDifference1k:
    case CodeCriticalRAM:
    case CodeArduinoCodeUnknown:
    case CodeGPSValueIndex:
    case CodeGPSCodeUnknown:
    case CodeAddingValueWithSameName:
    case CodePWMDriverNotAvailable:
    case CodePiLinkConnectionTimeOut:
    case CodeMainLoopCounterLowCritical:
    case CodeMotorCodeUnknown:
    case CodeBatteryCodeUnknown:
      return Error::Level::Critical;
    case CodeComputeTimeWarning:
    case CodeRAMDifference500:
    case CodeWarningRAM:
    case CodeMainLoopCounterLowWarning:
      return Error::Level::Warning;
    case CodeComputeTimeInfo:
    case CodeRAMDifference100:
    case CodeInfoRAM:
    case CodeMainLoopCounterLowInfo:
    case CodeNotStarted:
      return Error::Level::Info;
  };
  Error *error = new ArduinoError(ArduinoError::CodeArduinoCodeUnknown, NULL);
  ArduinoController::sharedController()->addError(error);
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
    case CodeRAMDifference1k:
    case CodeComputeTimeCritical:
    case CodeAddingValueWithSameName:
    case CodePWMDriverNotAvailable:
    case CodeMotorCodeUnknown:
    case CodeBatteryCodeUnknown:
      return true;
    case CodeComputeTimeInfo:
    case CodeComputeTimeWarning:
    case CodeRAMDifference100:
    case CodeRAMDifference500:
    case CodeInfoRAM:
    case CodeWarningRAM:
    case CodeCriticalRAM:
    case CodePiLinkConnectionTimeOut:
    case CodeMainLoopCounterLowInfo:
    case CodeMainLoopCounterLowWarning:
    case CodeMainLoopCounterLowCritical:
    case CodeNotStarted:
      return false;
  };
  Error *error = new ArduinoError(ArduinoError::CodeArduinoCodeUnknown, NULL);
  ArduinoController::sharedController()->addError(error);
  return false;
}
