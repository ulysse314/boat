#include "BatteryError.h"

#include "BatteryController.h"

BatteryError::BatteryError(Code code, const char *message) :
    Error(message),
    _code(code) {
}

BatteryError::~BatteryError() {
}

Error::Level BatteryError::getLevel() const {
  switch(_code) {
    case CodeNone:
    case CodeBatteryCodeUnknown:
    case CodeINA219NotFound:
    case CodeVoltageCritical:
    case CodeAmpereCritical:
    case CodeTemperatureUnknown:
    case CodeTemperatureCritical:
      return Error::Level::Critical;
    case CodeVoltageWarning:
    case CodeAmpereWarning:
    case CodeTemperatureWarning:
      return Error::Level::Warning;
    case CodeVoltageInfo:
    case CodeAmpereInfo:
    case CodeTemperatureInfo:
      return Error::Level::Info;
  };
  BatteryController::addBatteryError(BatteryError::CodeBatteryCodeUnknown);
  return Error::Level::Critical;
}

bool BatteryError::isPersistant() const {
  switch(_code) {
    case CodeNone:
    case CodeBatteryCodeUnknown:
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
      return false;
  };
  BatteryController::addBatteryError(BatteryError::CodeBatteryCodeUnknown);
  return false;
}
