#ifndef SensorList_h
#define SensorList_h

#include "Sensor.h"

typedef void *SensorCursor;

class SensorList
{
protected:
  typedef struct __SensorList {
    Sensor *sensor;
    struct __SensorList *next;
  } _SensorList;
  _SensorList *_firstSensor;

public:
  SensorList(void);
  
  void addSensor(Sensor *sensor);
  Sensor *sensorAtIndex(unsigned int index);
  unsigned int sensorCount(void);
  void *getSensorCursor(void) { return (void*)_firstSensor; };
  Sensor *getNextSensorCursor(void **sensorCursor);
  virtual boolean begin(void);
  virtual boolean areAllReady(void);
  virtual boolean loop(void);
  virtual boolean readValues(void);
  virtual boolean resetValues(void);
  virtual boolean printInfo(Stream *serial, int nodeID = -1);
  virtual boolean printAddress(Stream *serial);
  virtual boolean printValues(Stream *serial);
};

#endif // SensorList_h
