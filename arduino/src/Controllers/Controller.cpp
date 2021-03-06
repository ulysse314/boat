#include "Controller.h"

#include "../Controllers/ArduinoController.h"
#include "../Controllers/Value.h"
#include "../Main/Debug.h"

Controller::Controller() :
    _valueList(List<Value>::WhenDataRemoved::Free),
    _errorList(List<Error>::WhenDataRemoved::Free) {
}

Controller::~Controller() {}

bool Controller::addError(Error *error) {
  size_t index = _errorList.indexForData(error);
  if (index == (size_t)-1) {
    _errorList.addData(error);
    return true;
  } else {
    delete error;
  }
  return false;
}

bool Controller::removeError(Error *error) {
  bool result = _errorList.removeData(error);
  delete error;
  return result;
}

void Controller::clearNonPersistantErrors() {
  const List<Error>::Bucket *bucket = _errorList.firstBucket();
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
  const List<Error>::Bucket *bucket = _errorList.firstBucket();
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
    Error *error = new ArduinoError(ArduinoError::CodeAddingValueWithSameName, NULL);
    ArduinoController::sharedController()->addError(error);
    return;
  }
  _valueList.addData(value);
}
