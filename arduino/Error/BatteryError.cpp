#include "BatteryError.h"

#include "ArduinoController.h"

BatteryError::BatteryError(Code code, const char *message) :
    Error(message),
    _code(code) {
}

BatteryError::~BatteryError() {
}

Error::Level BatteryError::getLevel() const {
  switch(_code) {
    case CodeNone:
    case CodeINA219NotFound:
    case CodeVoltageCritical:
    case CodeAmpereCritical:
    case CodeTemperatureUnknown:
    case CodeTemperatureCritical:
      return Error::Level::Critical;
    case CodeVoltageWarning:
    case CodeAmpereWarning:
    case CodeTemperatureWarning:
    case CodeADS1115NotFound:
      return Error::Level::Warning;
    case CodeVoltageInfo:
    case CodeAmpereInfo:
    case CodeTemperatureInfo:
      return Error::Level::Info;
  };
  Error *error = new ArduinoError(ArduinoError::CodeBatteryCodeUnknown, NULL);
  ArduinoController::sharedController()->addError(error);
  return Error::Level::Critical;
}

bool BatteryError::isPersistant() const {
  switch(_code) {
    case CodeNone:
      return true;
    case CodeINA219NotFound:
    case CodeVoltageInfo:
    case CodeVoltageWarning:
    case CodeVoltageCritical:
    case CodeAmpereInfo:
    case CodeAmpereWarning:
    case CodeAmpereCritical:
    case CodeTemperatureUnknown:
    case CodeTemperatureInfo:
    case CodeTemperatureWarning:
    case CodeTemperatureCritical:
    case CodeADS1115NotFound:
      return false;
  };
  Error *error = new ArduinoError(ArduinoError::CodeBatteryCodeUnknown, NULL);
  ArduinoController::sharedController()->addError(error);
  return false;
}
