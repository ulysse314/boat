#ifndef PiLink_h
#define PiLink_h

#include <Arduino.h>

class Controller;
class Error;
class Value;

class PiLink {
public:
  PiLink(Stream *stream);

  void outputController(const Controller *controller);

protected:
  void outputValue(const Value *value);
  void outputError(const Error *error);

  Stream *_stream;
};

#endif // PiLink_h
