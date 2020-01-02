#ifndef Error_h
#define Error_h

#include <stddef.h>

class Error {
public:
  enum class Level {
    Debug,
    Info,
    Warning,
    Critical,
  };
  typedef enum {
    DomainNone,                               // 0
    DomainArduino,                            // 1
    DomainGPS,                                // 2
    DomainMotor,                              // 3
    DomainBattery,                            // 4
    DomainCellular,                           // 5 (only used in python code)
    DomainPi,                                 // 6 (only used in python code)
    DomainHull,                               // 7
  } Domain;

  Error(const char *message = NULL);
  virtual ~Error();

  virtual Domain getDomain() const = 0;
  virtual int getCode() const = 0;
  virtual Level getLevel() const = 0;
  virtual bool isPersistant() const = 0;
  const char *getMessage() const { return _message; };

  bool operator==(const Error &a) const {
    return getDomain() == a.getDomain() && getCode() == a.getCode();
  }
  
protected:
  Domain _domain;
  int _code;
  const char *const _message;
  bool _persistant;
};

#endif // Error_h
