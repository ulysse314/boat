#include "ActuatorList.h"

#include "Actuator.h"

inline bool isOrdered(Actuator *actuator1, Actuator *actuator2) {
  return strcmp(actuator1->getAddress(), actuator2->getAddress()) > 0;
}

class ActuatorSearcher {
protected:
  ActuatorList::ActuatorBucket *_bucket = NULL;
  size_t _characterIndex = 0;
  bool _searchFinished;

public:
  void reset(ActuatorList::ActuatorBucket *bucket) {
    _bucket = bucket;
    _characterIndex = 0;
    _searchFinished = false;
  }
  
  void nextCharacter(char character) {
    while (_bucket && _bucket->actuator->getAddress()[_characterIndex] < character) {
      _bucket = _bucket->nextBucket;
    }
    _characterIndex++;
  }

  Actuator *actuatorFound() const {
    if (_searchFinished && _bucket && _bucket->actuator->getAddress()[_characterIndex] == 0) {
      return _bucket->actuator;
    }
    return NULL;
  }
  
  void finished() {
    _searchFinished = true;
  }
  
  bool isFinished() const {
    return _searchFinished;
  }
};

ActuatorList::ActuatorList() {
  _actuatorBucket = NULL;
  _currentValue.reserve(128);
}

ActuatorList::~ActuatorList() {
  delete _actuatorSearcher;
  while(_actuatorBucket) {
    delete _actuatorBucket->actuator;
    ActuatorBucket *nextBucket = _actuatorBucket->nextBucket;
    delete _actuatorBucket;
    _actuatorBucket = nextBucket;
  }
}

void ActuatorList::addActuator(Actuator *actuator) {
  ActuatorBucket *newActuatorBucket = (ActuatorBucket *)malloc(sizeof(ActuatorBucket));
  newActuatorBucket->actuator = actuator;
  newActuatorBucket->nextBucket = NULL;
  
  ActuatorBucket **bucketCursor = &_actuatorBucket;
  while (*bucketCursor) {
    if (!isOrdered((*bucketCursor)->actuator, newActuatorBucket->actuator)) {
      break;
    }
    bucketCursor = &((*bucketCursor)->nextBucket);
  }
  newActuatorBucket->nextBucket = *bucketCursor;
  *bucketCursor = newActuatorBucket;
}

void ActuatorList::begin() {
  _actuatorSearcher = new ActuatorSearcher();
  ActuatorBucket *bucketCursor = _actuatorBucket;
  while (bucketCursor) {
    bucketCursor->actuator->begin();
    bucketCursor = bucketCursor->nextBucket;
  }
}

void ActuatorList::loop() {
  ActuatorBucket *bucketCursor = _actuatorBucket;
  while (bucketCursor) {
    bucketCursor->actuator->loop();
    bucketCursor = bucketCursor->nextBucket;
  }
}

void ActuatorList::listen(Stream *serial) {
  char buffer[30];
  size_t readAvailable;
  char *toProcess = buffer;
  while (readAvailable = serial->readBytes(buffer, sizeof(buffer) + 1)) {
    char *cursor = buffer;
    while (readAvailable) {
      while ((cursor[0] != '\n' && cursor[0] != '\r' && cursor[0] != ' ') && readAvailable) {
        if (!_actuatorSearcher->isFinished()) {
          _actuatorSearcher->nextCharacter(cursor[0]);
        }
        cursor++;
        readAvailable--;
      }
      if (!_actuatorSearcher->isFinished()) {
        // If there are still available characters, it must be a space, so the address is done.
        if (readAvailable) {
          _actuatorSearcher->finished();
        }
      } else {
        toProcess[cursor - toProcess] = 0;
        _currentValue.concat(toProcess);
        if (readAvailable) {
        // If there are still available characters, it must be a EOL.
          processBuffer();
        }
      }
      if (readAvailable) {
        readAvailable--;
        cursor++;
      }
      toProcess = cursor;
    }
  }
}

void ActuatorList::processBuffer() { 
  Actuator *actuatorFound = _actuatorSearcher->actuatorFound();
  if (actuatorFound) {
    actuatorFound->processValues(_currentValue.c_str());
  }
  _actuatorSearcher->reset(_actuatorBucket);
  _currentValue = "";
}
