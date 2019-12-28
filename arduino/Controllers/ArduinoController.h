#ifndef ArduinoController_h
#define ArduinoController_h

#include "ArduinoError.h"
#include "Controller.h"
#include "Value.h"

class HardwareConfig;
class OneWire;
class TwoWire;

class ArduinoController : public Controller {
public:
  static ArduinoController *generateController(HardwareConfig *hardwareConfig);
  static ArduinoController *sharedController();
  
  ~ArduinoController();

  void setCycleCount(long int cycleCount) { _cycleCount.setInteger(cycleCount); };
  void setLoopDuration(long int loopDuration) { _loopDuration.setInteger(loopDuration); };
  void setComputeTime(long int computeTime) { _computeTime.setInteger(computeTime); };
  void setCommand(const char *command);

  // Controller
  const char *getName() const override { return "arduino"; };
  void begin() override;
  void sensorsHasBeenUpdated() override;

protected:
  ArduinoController(HardwareConfig *hardwareConfig);

  HardwareConfig *_hardwareConfig;
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
