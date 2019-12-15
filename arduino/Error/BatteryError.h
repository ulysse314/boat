#ifndef BatteryError_h
#define BatteryError_h

#include "Error.h"

class BatteryError : public Error {
public:
  typedef enum {
    CodeNone,                          // 0
    CodeINA219NotFound,                // 1
    CodeVoltageInfo,                   // 2
    CodeVoltageWarning,                // 3
    CodeVoltageCritical,               // 4
    CodeAmpereInfo,                    // 5
    CodeAmpereWarning,                 // 6
    CodeAmpereCritical,                // 7
    CodeTemperatureUnknown,            // 8
    CodeTemperatureInfo,               // 9
    CodeTemperatureWarning,            // 10
    CodeTemperatureCritical,           // 11
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
