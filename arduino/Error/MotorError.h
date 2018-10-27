#ifndef MotorError_h
#define MotorError_h

#include "Error.h"

class MotorError : public Error {
public:
  typedef enum {
    CodeNone,                              // 0
    CodeTemperatureUnknown,                // 1
    CodeTemperatureInfo,                   // 2
    CodeTemperatureWarning,                // 3
    CodeTemperatureCritical,               // 4
    CodePWMNotAvailable,                   // 5
  } Code;
  
  MotorError(Code code, const char *message = NULL);
  ~MotorError();

  virtual Error::Domain getDomain() const override { return Error::DomainMotor; };
  virtual int getCode() const override { return _code; };
  virtual Level getLevel() const override;
  virtual bool isPersistant() const override { return false; };

private:
  Code _code;
};

#endif // MotorError_h
