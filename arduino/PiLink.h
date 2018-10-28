#ifndef PiLink_h
#define PiLink_h

#include <Arduino.h>

class Controller;
class Error;
class PWMDriver;
class Value;

#define InputBufferSize 64

class PiLink {
public:
  static PiLink *getPiLink();

  void setLeftMotorController(Controller *motorController) { _leftMotorController = motorController; };
  void setRightMotorController(Controller *motorController) { _rightMotorController = motorController; };

  void outputController(const Controller *controller);
  void listen();
  bool hasTimedOut();

protected:
  PiLink(Stream *stream);

  void outputValue(const Value *value);
  void outputError(const Error *error);

  void processInputBuffer();

  Stream *_stream;
  char _inputBuffer[InputBufferSize];
  size_t _inputBufferLength;
  unsigned long int _nextTimeOut;

  Controller *_leftMotorController;
  Controller *_rightMotorController;
};

#endif // PiLink_h
