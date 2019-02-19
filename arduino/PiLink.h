#ifndef PiLink_h
#define PiLink_h

#include <Arduino.h>

class ArduinoController;
class Controller;
class Error;
class MotorController;
class PWMDriver;
class Value;

#define InputBufferSize 64

class PiLink {
public:
  static PiLink *getPiLink();

  void setLeftMotorController(MotorController *motorController) { _leftMotorController = motorController; };
  void setRightMotorController(MotorController *motorController) { _rightMotorController = motorController; };
  void setArduinoController(ArduinoController *arduinoController) { _arduinoController = arduinoController; };

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

  MotorController *_leftMotorController;
  MotorController *_rightMotorController;
  ArduinoController *_arduinoController;
};

#endif // PiLink_h
