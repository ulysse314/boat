#ifndef HullError_h
#define HullError_h

#include "Error.h"

class HullError : public Error {
public:
  typedef enum {
    CodeNone,                          // 0
    CodeADS1115NotFound,               // 1
    CodeLeak,                          // 2
    TemperatureInfo,                   // 3
    TemperatureWarning,                // 4
    TemperatureCritical,               // 5
    TemperatureInvalid,                // 6
  } Code;
  
  HullError(Code code, const char *message = NULL);
  ~HullError();

  virtual Error::Domain getDomain() const override { return Error::DomainHull; };
  virtual int getCode() const override { return _code; };
  virtual Level getLevel() const override;
  virtual bool isPersistant() const override;

private:
  Code _code;
};

#endif // HullError_h
