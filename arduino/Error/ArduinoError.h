#ifndef ArduinoError_h
#define ArduinoError_h

#include "Error.h"

class ArduinoError : public Error {
public:
  typedef enum {
    CodeNone,                          // 0
    CodeSetValueWithWrongType,         // 1
    CodeGetValueWithWrongType,         // 2
    CodeComputeTimeInfo,               // 3
    CodeComputeTimeWarning,            // 4
    CodeComputeTimeCritical,           // 5
    CodeRAMDifference100,              // 6
    CodeRAMDifference500,              // 7
    CodeRAMDifference1k,               // 8
    CodeInfoRAM,                       // 9
    CodeWarningRAM,                    // 10
    CodeCriticalRAM,                   // 11
    CodeArduinoCodeUnknown,            // 12
    CodeGPSValueIndex,                 // 13
    CodeGPSCodeUnknown,                // 14
    CodeAddingValueWithSameName,       // 15
    CodePWMDriverNotAvailable,         // 16
    CodePiLinkConnectionTimeOut,       // 17
    CodeMainLoopCounterLowInfo,        // 18
    CodeMainLoopCounterLowWarning,     // 19
    CodeMainLoopCounterLowCritical,    // 20
    CodeNotStarted,                    // 21
  } Code;
  
  ArduinoError(Code code, const char *message = NULL);
  ~ArduinoError();

  virtual Error::Domain getDomain() const override { return Error::DomainArduino; };
  virtual int getCode() const override { return _code; };
  virtual Level getLevel() const override;
  virtual bool isPersistant() const override;

private:
  Code _code;
};

#endif // ArduinoError_h
