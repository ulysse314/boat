#ifndef InfoActuatorSensor_h
#define InfoActuatorSensor_h

#include <Arduino.h>

#include "Actuator.h"
#include "Sensor.h"

class InfoActuatorSensor :public Actuator, public Sensor {
public:
  const char *sensorClass() const override { return "Info"; };
  const char *sensorType() const  override { return "Info"; };
  const char *addressString() const override { return "Info"; };
  bool readValues() override;
  void loop() override;
  bool printInfo(Stream *serial, int nodeID = -1)  override;
  bool printValues(Stream *serial) override;

  bool processValues(const char *values) override;
  
  void setCycleCount(unsigned long count) { _cycleCount = count; };
  void setLoopDuration(unsigned long duration) { _loopDuration = duration; };

protected:
  bool _needPrintInfo = true;
  unsigned long _timestamp = 0;
  unsigned long _lastMillis = 0;
  unsigned long _freeRAM = 0;
  long _ramDifference = 0;
  unsigned long _cycleCount;
  unsigned long _loopDuration;
};

#endif // InfoActuatorSensor_h
