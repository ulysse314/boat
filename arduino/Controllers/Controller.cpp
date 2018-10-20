#include "Controller.h"

#include "ArduinoController.h"
#include "Debug.h"
#include "Value.h"

Controller::Controller() :
    _valueList(List<Value>::WhenDataRemoved::Free),
    _errorList(List<Error>::WhenDataRemoved::Free) {
}

Controller::~Controller() {}

void Controller::addError(Error *error) {
  size_t index = _errorList.indexForData(error);
  if (index == (size_t)-1) {
    _errorList.addData(error);
  }
}

void Controller::clearNonPersistantErrors() {
  const List<Error>::Bucket *bucket = _errorList.getFirstBucket();
  Error *error = NULL;
  while (_errorList.currentDataWithBucket(bucket, error)) {
    if (error->isPersistant()) {
      _errorList.nextDataWithBucket(bucket, error);
    } else {
      _errorList.removeCurrentDataWithNextBucket(bucket);
    }
  }
}

bool Controller::removeErrorWithCode(Error::Domain domain, int code) {
  bool result = false;
  const List<Error>::Bucket *bucket = _errorList.getFirstBucket();
  Error *error = NULL;
  while (_errorList.currentDataWithBucket(bucket, error)) {
    if (error->getDomain() == domain && error->getCode() == code) {
      _errorList.removeCurrentDataWithNextBucket(bucket);
      result = true;
    } else {
      _errorList.nextDataWithBucket(bucket, error);
    }
  }
  return result;
}

void Controller::addValue(Value *value) {
  size_t index = _valueList.indexForData(value);
  if (index != (size_t)-1) {
    ArduinoController::addArduinoError(ArduinoError::CodeAddingValueWithSameName);
    return;
  }
  _valueList.addData(value);
}
