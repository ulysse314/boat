#ifndef PiLink_h
#define PiLink_h

#include <Arduino.h>

class Controller;
class Error;

class PiLink {
public:
  PiLink(Stream *stream);

  void outputController(const Controller *controller);
  void outputError(const Error *error);

protected:
  Stream *_stream;
};

#endif // PiLink_h
