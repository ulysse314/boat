#include "Value.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "ArduinoController.h"
#include "Error.h"
#include "StringUtils.h"

Value::Value(Type type, const char *name) :
    _type(type),
    _name(createStringCopy(name)),
    _string(NULL),
    _integer(0),
    _double(0),
    _boolean(false) {
}

Value::~Value() {
  if (_string) {
    free((void *)_string);
  }
}

void Value::setNull() {
  _null = true;
}

void Value::setString(const char *value) {
  if (_type != Type::String) {
    Error *error = new ArduinoError(ArduinoError::CodeSetValueWithWrongType, NULL);
    ArduinoController::sharedController()->addError(error);
    return;
  }
  if (_string) {
    free((void *)_string);
  }
  if (!value) {
    _string = NULL;
    return;
  }
  _string = createStringCopy(value);
  _null = false;
}

void Value::setInteger(int32_t value) {
  if (_type != Type::Integer) {
    Error *error = new ArduinoError(ArduinoError::CodeSetValueWithWrongType, NULL);
    ArduinoController::sharedController()->addError(error);
    return;
  }
  _integer = value;
  _null = false;
}

void Value::setDouble(double value) {
  if (_type != Type::Double) {
    Error *error = new ArduinoError(ArduinoError::CodeSetValueWithWrongType, NULL);
    ArduinoController::sharedController()->addError(error);
    return;
  }
  _double = value;
  _null = false;
}

void Value::setBoolean(bool value) {
  if (_type != Type::Boolean) {
    Error *error = new ArduinoError(ArduinoError::CodeSetValueWithWrongType, NULL);
    ArduinoController::sharedController()->addError(error);
    return;
  }
  _boolean = value;
  _null = false;
}

const char *Value::valueAsString() const {
  if (_type != Type::String) {
    Error *error = new ArduinoError(ArduinoError::CodeGetValueWithWrongType, NULL);
    ArduinoController::sharedController()->addError(error);
  }
  return _string;
}

int32_t Value::valueAsInteger() const {
  if (_type != Type::Integer) {
    Error *error = new ArduinoError(ArduinoError::CodeGetValueWithWrongType, NULL);
    ArduinoController::sharedController()->addError(error);
  }
  return _integer;
}

double Value::valueAsDouble() const {
  if (_type != Type::Double) {
    Error *error = new ArduinoError(ArduinoError::CodeGetValueWithWrongType, NULL);
    ArduinoController::sharedController()->addError(error);
  }
  return _double;
}

bool Value::valueAsBoolean() const {
  if (_type != Type::Boolean) {
    Error *error = new ArduinoError(ArduinoError::CodeGetValueWithWrongType, NULL);
    ArduinoController::sharedController()->addError(error);
  }
  return _boolean;
}
