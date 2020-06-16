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

  void setLoopCounter(long int cycleCount) { _loopCounter.setInteger(cycleCount); };
  void setLongestLoopDuration(long int longestLoopDuration) { _longestLoopDuration.setInteger(longestLoopDuration); };
  void setCycleDuration(long int cycleDuration) { _cycleDuration.setInteger(cycleDuration); };
  void setExportDuration(long int exportDuration) { _exportDuration.setInteger(exportDuration); };
  void setCommand(const char *command);

  // Controller
  const char *name() const override { return "ard"; };
  void begin() override;
  void sensorsHasBeenUpdated() override;

protected:
  ArduinoController(HardwareConfig *hardwareConfig);

  HardwareConfig *_hardwareConfig;
  size_t _infoFreeRAM;
  size_t _warningFreeRAM;
  size_t _criticalFreeRAM;
  unsigned long _lastMillis;
  // Counter for each cycle.
  unsigned long _cylcleCounter;
  Value _started;
  Value _loopCounter;
  Value _longestLoopDuration;
  Value _cycleDuration;
  Value _exportDuration;
  Value _ramFree;
  Value _ramFreeDifference;
  Value _millis;
  Value _timestamp;
  Value _compileDate;
  Value _arduinoVersion;
  Value _debugInfo;
};

#endif // ArduinoController_h
