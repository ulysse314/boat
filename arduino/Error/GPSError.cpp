#include "GPSError.h"

#include "ArduinoController.h"
#include "ArduinoError.h"

GPSError::GPSError(Code code, const char *message) :
    Error(message),
    _code(code) {
}

GPSError::~GPSError() {}

Error::Level GPSError::getLevel() const {
  switch(_code) {
    case CodeNone:
    case CodeUnknownAntenna:
    case CodeExternalAntennaProblem:
    case CodeNoFixMode:
    case CodeUsedSatellitesTooLow:
      return Error::Level::Critical;
    case CodeUsingInternalAntenna:
    case Code2DMode:
    case CodeUsedSatellitesLow:
      return Error::Level::Warning;
    
  };
  ArduinoController::addArduinoError(ArduinoError::CodeGPSCodeUnknown);
  return Error::Level::Critical;
}
