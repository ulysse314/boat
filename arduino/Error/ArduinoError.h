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
    CodeMemoryDifference100,           // 6
    CodeMemoryDifference500,           // 7
    CodeMemoryDifference1k,            // 8
    CodeInfoMemory,                    // 9
    CodeLowMemory,                     // 10
    CodeVeryLowMemory,                 // 11
    CodeArduinoCodeUnknown,            // 12
    CodeGPSValueIndex,                 // 13
    CodeGPSCodeUnknown,                // 14
    CodeAddingValueWithSameName,       // 15
    CodePWMDriverNotAvailable,         // 16
    CodePiLinkConnectionTimeOut,       // 17
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
