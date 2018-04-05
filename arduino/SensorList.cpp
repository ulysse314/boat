#include "SensorList.h"

SensorList::SensorList(void) {
  _firstSensor = NULL;
}

void SensorList::addSensor(Sensor *sensor) {
  _SensorList *newSensor;
  _SensorList **cursor;
  
  newSensor = (_SensorList *)malloc(sizeof(_SensorList));
  newSensor->sensor = sensor;
  newSensor->next = NULL;
  
  cursor = &_firstSensor;
  while (*cursor) {
    cursor = &((*cursor)->next);
  }
  *cursor = newSensor;
}

Sensor *SensorList::sensorAtIndex(unsigned int index) {
  _SensorList *cursor = _firstSensor;
  
  while (cursor && index > 0) {
    cursor = cursor->next;
    index--;
  }
  return cursor?cursor->sensor:NULL;
}

unsigned int SensorList::sensorCount(void) {
  _SensorList *cursor = _firstSensor;
  unsigned int count = 0;
  
  while (cursor) {
    cursor = cursor->next;
    count++;
  }
  return count;
}

Sensor *SensorList::getNextSensorCursor(void **sensorCursor) {
  Sensor *result;
  
  if (sensorCursor == NULL || *sensorCursor == NULL) {
    result = NULL;
  } else {
    _SensorList *list = (_SensorList *)*sensorCursor;
    result = list->sensor;
    *sensorCursor = (void *)(list->next);
  }
  return result;
}

boolean SensorList::begin(void) {
  boolean result = true;
  _SensorList *cursor = _firstSensor;
  
  while (cursor) {
    result = cursor->sensor->begin() && result;
    cursor = cursor->next;
  }
  return result;
}

boolean SensorList::loop(void) {
  boolean result = true;
  _SensorList *cursor = _firstSensor;
  
  while (cursor) {
    result = cursor->sensor->loop() && result;
    cursor = cursor->next;
  }
  return result;
}

boolean SensorList::readValues(void) {
  boolean result = true;
  _SensorList *cursor = _firstSensor;
  
  while (cursor) {
    result = cursor->sensor->readValues() && result;
    cursor = cursor->next;
  }
  return result;
}

boolean SensorList::resetValues(void) {
  boolean result = true;
  _SensorList *cursor = _firstSensor;
  
  while (cursor) {
    result = cursor->sensor->resetValues() && result;
    cursor = cursor->next;
  }
  return result;
}

boolean SensorList::printInfo(Stream *serial, int nodeID) {
  boolean result = true;
  _SensorList *cursor = _firstSensor;
  
  while (cursor) {
    result = cursor->sensor->printInfo(serial, nodeID) && result;
    cursor = cursor->next;
  }
  return result;
}

boolean SensorList::areAllReady(void) {
  boolean result = true;
  _SensorList *cursor = _firstSensor;
  
  while (cursor) {
    if (!cursor->sensor->isReady()) {
      result = false;
      break;
    }
    cursor = cursor->next;
  }
  return result;
}

boolean SensorList::printAddress(Stream *serial) {
  boolean result = true;
  _SensorList *cursor = _firstSensor;
  
  while (cursor) {
    result = cursor->sensor->printAddress(serial) && result;
    cursor = cursor->next;
  }
  return result;
}

boolean SensorList::printValues(Stream *serial) {
  boolean result = true;
  _SensorList *cursor = _firstSensor;
  
  while (cursor) {
    result = cursor->sensor->printValues(serial) && result;
    cursor = cursor->next;
  }
  return result;
}
