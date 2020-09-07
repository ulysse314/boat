#ifndef PiLink_h
#define PiLink_h

#include <Arduino.h>

class ArduinoController;
class BatteryController;
class Controller;
class Error;
class HardwareConfig;
class MotorController;
class PWMDriver;
class Value;

#define InputBufferSize 64

class PiLink {
public:
  static PiLink *generatePiLink(HardwareConfig *hardwareConfig);
  static PiLink *getSharedPiLink();

  void setLeftMotorController(MotorController *motorController) { _leftMotorController = motorController; };
  void setRightMotorController(MotorController *motorController) { _rightMotorController = motorController; };
  void setArduinoController(ArduinoController *arduinoController) { _arduinoController = arduinoController; };
  void setBatteryController(BatteryController *batteryController) { _batteryController = batteryController; };

  void outputController(const Controller *controller);
  void listen();
  bool hasTimedOut();

protected:
  PiLink(HardwareConfig *hardwareConfig);

  void outputValue(const Value *value);
  void outputError(const Error *error);

  void processInputBuffer();

  Stream *const _stream;
  char _inputBuffer[InputBufferSize];
  size_t _inputBufferLength;
  unsigned long int _nextTimeOut;

  MotorController *_leftMotorController;
  MotorController *_rightMotorController;
  ArduinoController *_arduinoController;
  BatteryController *_batteryController;
};

#endif // PiLink_h
