#ifndef SensorList_h
#define SensorList_h

#include "Sensor.h"

typedef void *SensorCursor;

class SensorList
{
protected:
  typedef struct _SensorBucket {
    Sensor *sensor;
    struct _SensorBucket *nextBucket;
  } SensorBucket;
  SensorBucket *_sensorBucket;

public:
  SensorList();
  ~SensorList();
  
  void addSensor(Sensor *sensor);
  Sensor *sensorAtIndex(unsigned int index);
  unsigned int sensorCount();
  void *getSensorCursor() { return (void*)_sensorBucket; };
  Sensor *getNextSensor(void **sensorCursor);
  virtual bool begin();
  virtual bool areAllReady();
  virtual bool loop();
  virtual bool readValues();
  virtual bool resetValues();
  virtual bool printInfo(Stream *serial, int nodeID = -1);
  virtual bool printAddress(Stream *serial);
  virtual bool printValues(Stream *serial);
};

#endif // SensorList_h
