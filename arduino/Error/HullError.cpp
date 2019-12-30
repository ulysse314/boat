#include "HullError.h"

#include "ArduinoController.h"

HullError::HullError(Code code, const char *message) :
    Error(message),
    _code(code) {
}

HullError::~HullError() {
}

Error::Level HullError::getLevel() const {
  switch(_code) {
    case CodeNone:
    case CodeADS1115NotFound:
    case CodeLeak:
    case TemperatureCritical:
      return Error::Level::Critical;
    case TemperatureWarning:
      return Error::Level::Warning;
    case TemperatureInfo:
      return Error::Level::Info;
  };
  Error *error = new ArduinoError(ArduinoError::CodeHullCodeUnknown, NULL);
  ArduinoController::sharedController()->addError(error);
  return Error::Level::Critical;
}

bool HullError::isPersistant() const {
  switch(_code) {
    case CodeNone:
      return true;
    case CodeADS1115NotFound:
    case CodeLeak:
    case TemperatureCritical:
    case TemperatureWarning:
    case TemperatureInfo:
      return false;
  };
  Error *error = new ArduinoError(ArduinoError::CodeHullCodeUnknown, NULL);
  ArduinoController::sharedController()->addError(error);
  return false;
}
