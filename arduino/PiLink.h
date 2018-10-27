#ifndef PiLink_h
#define PiLink_h

#include <Arduino.h>

class Controller;
class Error;
class Value;

#define InputBufferSize 64

class PiLink {
public:
  PiLink(Stream *stream);

  void setLeftMotorController(Controller *motorController) { _leftMotorController = motorController; };
  void setRightMotorController(Controller *motorController) { _rightMotorController = motorController; };

  void outputController(const Controller *controller);
  void listen();

protected:
  void outputValue(const Value *value);
  void outputError(const Error *error);

  void processInputBuffer();

  Stream *_stream;
  char _inputBuffer[InputBufferSize];
  size_t _inputBufferLength;

  Controller *_leftMotorController;
  Controller *_rightMotorController;
};

#endif // PiLink_h
