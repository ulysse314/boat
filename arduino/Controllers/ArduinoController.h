#ifndef ArduinoController_h
#define ArduinoController_h

#include "ArduinoError.h"
#include "Controller.h"
#include "Value.h"

class OneWire;
class TwoWire;

class ArduinoController : public Controller {
public:
  static ArduinoController *getArduinoController();
  static bool addArduinoError(ArduinoError::Code code);
  static bool removeArduinoError(ArduinoError::Code code);
  
public:
  ArduinoController(TwoWire *ic2, OneWire *oneWire);

  void setCycleCount(long int cycleCount) { _cycleCount.setInteger(cycleCount); };
  void setLoopDuration(long int loopDuration) { _loopDuration.setInteger(loopDuration); };
  void setComputeTime(long int computeTime) { _computeTime.setInteger(computeTime); };
  void setCommand(const char *command);

  // Controller
  const char *getName() const override { return "arduino"; };
  void begin() override;
  void sensorsHasBeenUpdated() override;

protected:
  TwoWire *_ic2;
  OneWire *_oneWire;
  size_t _infoFreeRAM;
  size_t _warningFreeRAM;
  size_t _criticalFreeRAM;
  unsigned long _lastMillis;
  unsigned long _loopCount;
  Value _started;
  Value _cycleCount;
  Value _loopDuration;
  Value _computeTime;
  Value _ramFree;
  Value _ramFreeDifference;
  Value _millis;
  Value _timestamp;
  Value _compileDate;
  Value _arduinoVersion;
  Value _debugInfo;
};

#endif // ArduinoController_h
