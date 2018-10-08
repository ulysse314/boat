#include "SensorList.h"

SensorList::SensorList() {
  _sensorBucket = NULL;
}

SensorList::~SensorList() {
  while (_sensorBucket) {
    delete _sensorBucket->sensor;
    SensorBucket *nextBucket = _sensorBucket->nextBucket;
    delete _sensorBucket;
    _sensorBucket = nextBucket;
  }
}

void SensorList::addSensor(Sensor *sensor) {
  SensorBucket *newSensorBucket;
  SensorBucket **bucketCursor;
  
  newSensorBucket = (SensorBucket *)malloc(sizeof(SensorBucket));
  newSensorBucket->sensor = sensor;
  newSensorBucket->nextBucket = NULL;
  
  bucketCursor = &_sensorBucket;
  while (*bucketCursor) {
    bucketCursor = &((*bucketCursor)->nextBucket);
  }
  *bucketCursor = newSensorBucket;
}

Sensor *SensorList::sensorAtIndex(unsigned int index) {
  SensorBucket *bucketCursor = _sensorBucket;
  
  while (bucketCursor && index > 0) {
    bucketCursor = bucketCursor->nextBucket;
    index--;
  }
  return bucketCursor?bucketCursor->sensor:NULL;
}

unsigned int SensorList::sensorCount(void) {
  SensorBucket *bucketCursor = _sensorBucket;
  unsigned int count = 0;
  
  while (bucketCursor) {
    bucketCursor = bucketCursor->nextBucket;
    count++;
  }
  return count;
}

Sensor *SensorList::getNextSensor(void **cursor) {
  Sensor *result;
  
  if (cursor == NULL || *cursor == NULL) {
    result = NULL;
  } else {
    SensorBucket *bucketCursor = (SensorBucket *)*cursor;
    result = bucketCursor->sensor;
    *cursor = (void *)(bucketCursor->nextBucket);
  }
  return result;
}

bool SensorList::begin() {
  bool result = true;
  SensorBucket *bucketCursor = _sensorBucket;
  
  while (bucketCursor) {
    result = bucketCursor->sensor->begin() && result;
    bucketCursor = bucketCursor->nextBucket;
  }
  return result;
}

bool SensorList::loop() {
  bool result = true;
  SensorBucket *bucketCursor = _sensorBucket;
  
  while (bucketCursor) {
    result = bucketCursor->sensor->loop() && result;
    bucketCursor = bucketCursor->nextBucket;
  }
  return result;
}

bool SensorList::readValues() {
  bool result = true;
  SensorBucket *bucketCursor = _sensorBucket;
  
  while (bucketCursor) {
    result = bucketCursor->sensor->readValues() && result;
    bucketCursor = bucketCursor->nextBucket;
  }
  return result;
}

bool SensorList::resetValues() {
  bool result = true;
  SensorBucket *bucketCursor = _sensorBucket;
  
  while (bucketCursor) {
    result = bucketCursor->sensor->resetValues() && result;
    bucketCursor = bucketCursor->nextBucket;
  }
  return result;
}

bool SensorList::printInfo(Stream *serial, int nodeID) {
  bool result = true;
  SensorBucket *bucketCursor = _sensorBucket;
  
  while (bucketCursor) {
    result = bucketCursor->sensor->printInfo(serial, nodeID) && result;
    bucketCursor = bucketCursor->nextBucket;
  }
  return result;
}

bool SensorList::areAllReady() {
  bool result = true;
  SensorBucket *bucketCursor = _sensorBucket;
  
  while (bucketCursor) {
    if (!bucketCursor->sensor->isReady()) {
      result = false;
      break;
    }
    bucketCursor = bucketCursor->nextBucket;
  }
  return result;
}

bool SensorList::printAddress(Stream *serial) {
  bool result = true;
  SensorBucket *bucketCursor = _sensorBucket;
  
  while (bucketCursor) {
    result = bucketCursor->sensor->printAddress(serial) && result;
    bucketCursor = bucketCursor->nextBucket;
  }
  return result;
}

bool SensorList::printValues(Stream *serial) {
  bool result = true;
  SensorBucket *bucketCursor = _sensorBucket;
  
  while (bucketCursor) {
    result = bucketCursor->sensor->printValues(serial) && result;
    bucketCursor = bucketCursor->nextBucket;
  }
  return result;
}