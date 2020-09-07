#ifndef GPSError_h
#define GPSError_h

#include "../Errors/Error.h"

class GPSError : public Error {
public:
  typedef enum {
    CodeNone,                              // 0
    CodeUnknownAntenna,                    // 1
    CodeExternalAntennaProblem,            // 2
    CodeUsingInternalAntenna,              // 3
    CodeUnknownMode,                       // 4
    CodeNoFixMode,                         // 5
    Code2DMode,                            // 6
    CodeUsedSatellitesTooLow,              // 7
    CodeUsedSatellitesLow,                 // 8
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
