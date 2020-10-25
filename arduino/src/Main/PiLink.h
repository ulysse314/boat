#ifndef PiLink_h
#define PiLink_h

#include <Arduino.h>

class ArduinoController;
class AutoPilot;
class BatteryController;
class Controller;
class Error;
class HardwareConfig;
class LidController;
class MotorController;
class PWMDriver;
class Value;

#define InputBufferSize 64

class PiLink {
public:
  static PiLink *generatePiLink(HardwareConfig *hardwareConfig);
  static PiLink *getSharedPiLink();

  void setArduinoController(ArduinoController *arduinoController) { _arduinoController = arduinoController; };
  void setBatteryController(BatteryController *batteryController) { _batteryController = batteryController; };
  void setLidController(LidController *lidController) { _lidController = lidController; };
  void setLeftMotorController(MotorController *motorController) { _leftMotorController = motorController; };
  void setRightMotorController(MotorController *motorController) { _rightMotorController = motorController; };
  void setAutoPilot(AutoPilot *autoPilot) { _autoPilot = autoPilot; };

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

  ArduinoController *_arduinoController;
  BatteryController *_batteryController;
  LidController *_lidController;
  MotorController *_leftMotorController;
  MotorController *_rightMotorController;
  AutoPilot *_autoPilot;
};

#endif // PiLink_h
