#include "MotorError.h"

#include "../Controllers/ArduinoController.h"

MotorError::MotorError(Code code, const char *message) :
    Error(message),
    _code(code) {
}

MotorError::~MotorError() {}

Error::Level MotorError::getLevel() const {
  switch(_code) {
    case CodeNone:
    case CodeTemperatureCritical:
    case CodePWMNotAvailable:
      return Error::Level::Critical;
    case CodeTemperatureUnknown:
    case CodeTemperatureWarning:
      return Error::Level::Warning;
    case CodeTemperatureInfo:
      return Error::Level::Info;
  };
  Error *error = new ArduinoError(ArduinoError::CodeMotorCodeUnknown, NULL);
  ArduinoController::sharedController()->addError(error);
  return Error::Level::Critical;
}
