#ifndef ArduinoController_h
#define ArduinoController_h

#include "ArduinoError.h"
#include "Controller.h"
#include "Value.h"

class ArduinoController : public Controller {
public:
  enum ValueIndex {
    StartedIndex = 0,
    CycleCountIndex,
    LoopDurationIndex,
    ComputeTimeIndex,
    RAMFreeIndex,
    RAMFreeDifferenceIndex,
    MillisIndex,
    TimestampIndex,
    CompileDateIndex,
    ArduinoVersionIndex,
    MaxIndex,
  };

  static ArduinoController *getArduinoController();
  static void addArduinoError(ArduinoError::Code code);
  
public:
  void setCycleCount(long int cycleCount) { _cycleCount.setInteger(cycleCount); };
  void setLoopDuration(long int loopDuration) { _loopDuration.setInteger(loopDuration); };
  void setComputeTime(long int computeTime) { _computeTime.setInteger(computeTime); };

  // Controller
  const char *getName() const override { return "arduino"; };
  void begin();
  int getValueCount() const override{ return MaxIndex; };
  const Value *getValueAtIndex(int) const override;
  void sensorsHasBeenUpdated() override;

protected:
  ArduinoController();

  size_t _infoFreeMemory;
  size_t _lowFreeMemory;
  size_t _veryLowFreeMemory;
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
};

#endif // ArduinoController_h
