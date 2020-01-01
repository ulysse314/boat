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
    case CodeExportDurationCritical:
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
    case CodeHullCodeUnknown:
      return Error::Level::Critical;
    case CodeExportDurationWarning:
    case CodeRAMDifference500:
    case CodeWarningRAM:
    case CodeMainLoopCounterLowWarning:
      return Error::Level::Warning;
    case CodeExportDurationInfo:
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
    case CodeExportDurationCritical:
    case CodeAddingValueWithSameName:
    case CodePWMDriverNotAvailable:
    case CodeMotorCodeUnknown:
    case CodeBatteryCodeUnknown:
    case CodeHullCodeUnknown:
      return true;
    case CodeExportDurationInfo:
    case CodeExportDurationWarning:
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
