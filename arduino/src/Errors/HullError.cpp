#include "HullError.h"

#include "../Controllers/ArduinoController.h"

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
    case TemperatureInvalid:
    case BNO055Error:
    case BNO055SystemStatus:
      return Error::Level::Critical;
    case TemperatureWarning:
    case BNO055SelfTest:
    case BNO055SysCalibration:
    case BNO055AccelCalibration:
    case BNO055GyroCalibration:
    case BNO055MagCalibration:
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
    case TemperatureInfo:
    case TemperatureWarning:
    case TemperatureCritical:
    case TemperatureInvalid:
    case BNO055Error:
    case BNO055SystemStatus:
    case BNO055SelfTest:
    case BNO055SysCalibration:
    case BNO055AccelCalibration:
    case BNO055GyroCalibration:
    case BNO055MagCalibration:
      return false;
  };
  Error *error = new ArduinoError(ArduinoError::CodeHullCodeUnknown, NULL);
  ArduinoController::sharedController()->addError(error);
  return false;
}
