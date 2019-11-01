#ifndef GPSError_h
#define GPSError_h

#include "Error.h"

class GPSError : public Error {
public:
  typedef enum {
    CodeNone,                              // 0
    CodeUnknownAntenna,                    // 1
    CodeExternalAntennaProblem,            // 2
    CodeUsingInternalAntenna,              // 3
    CodeNoFixMode,                         // 4
    Code2DMode,                            // 5
    CodeUsedSatellitesTooLow,              // 6
    CodeUsedSatellitesLow,                 // 7
  } Code;
  
  GPSError(Code code, const char *message = NULL);
  ~GPSError();

  virtual Error::Domain getDomain() const override { return Error::DomainGPS; };
  virtual int getCode() const override { return _code; };
  virtual Level getLevel() const override;
  virtual bool isPersistant() const override { return false; };

private:
  Code _code;
};

#endif // GPSError_h
