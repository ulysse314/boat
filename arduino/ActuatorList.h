#ifndef ActuatorList_h
#define ActuatorList_h

#include <Arduino.h>

class Actuator;
class ActuatorSearcher;

class ActuatorList {
protected:
  typedef struct _ActuatorBucket {
    Actuator *actuator;
    struct _ActuatorBucket *nextBucket;
  } ActuatorBucket;
  ActuatorBucket *_actuatorBucket;
  ActuatorBucket *_currentActuatorBucket;
  ActuatorSearcher *_actuatorSearcher;
  String _currentValue;

public:
  ActuatorList();
  ~ActuatorList();
  void begin();
  void addActuator(Actuator *actuator);
  void loop();
  void listen(Stream *serial);

protected:
  void processBuffer();
  friend class ActuatorSearcher;
};

#endif // ActuatorList_h
