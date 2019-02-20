#ifndef BatteryError_h
#define BatteryError_h

#include "Error.h"

class BatteryError : public Error {
public:
  typedef enum {
    CodeNone,                          // 0
    CodeBatteryCodeUnknown,            // 1
    CodeINA219NotFound,                // 2
    CodeVoltageInfo,                   // 3
    CodeVoltageWarning,                // 4
    CodeVoltageCritical,               // 5
    CodeAmpereInfo,                    // 6
    CodeAmpereWarning,                 // 7
    CodeAmpereCritical,                // 8
    CodeTemperatureUnknown,            // 9
    CodeTemperatureInfo,               // 10
    CodeTemperatureWarning,            // 11
    CodeTemperatureCritical,           // 12
  } Code;
  
  BatteryError(Code code, const char *message = NULL);
  ~BatteryError();

  virtual Error::Domain getDomain() const override { return Error::DomainBattery; };
  virtual int getCode() const override { return _code; };
  virtual Level getLevel() const override;
  virtual bool isPersistant() const override;

private:
  Code _code;
};

#endif // BatteryError_h
