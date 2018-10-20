#include "Value.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "ArduinoController.h"
#include "Error.h"

Value::Value(Type type, const char *name) :
    _type(type),
    _name(NULL),
    _string(NULL),
    _integer(0),
    _double(0),
    _boolean(false) {
  size_t length = strlen(name) + 1;
  char *stringCopy = (char *)malloc(length);
  strncpy(stringCopy, name, length);
  _name = stringCopy;
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
    ArduinoController::addArduinoError(ArduinoError::CodeSetValueWithWrongType);
    return;
  }
  if (_string) {
    free((void *)_string);
  }
  if (!value) {
    _string = NULL;
    return;
  }
  size_t length = strlen(value) + 1;
  char *stringCopy = (char *)malloc(length);
  strncpy(stringCopy, value, length);
  _string = stringCopy;
  _null = false;
}

void Value::setInteger(int32_t value) {
  if (_type != Type::Integer) {
    ArduinoController::addArduinoError(ArduinoError::CodeSetValueWithWrongType);
    return;
  }
  _integer = value;
  _null = false;
}

void Value::setDouble(double value) {
  if (_type != Type::Double) {
    ArduinoController::addArduinoError(ArduinoError::CodeSetValueWithWrongType);
    return;
  }
  _double = value;
  _null = false;
}

void Value::setBoolean(bool value) {
  if (_type != Type::Boolean) {
    ArduinoController::addArduinoError(ArduinoError::CodeSetValueWithWrongType);
    return;
  }
  _boolean = value;
  _null = false;
}

const char *Value::getString() const {
  if (_type != Type::String) {
    ArduinoController::addArduinoError(ArduinoError::CodeGetValueWithWrongType);
  }
  return _string;
}

int32_t Value::getInteger() const {
  if (_type != Type::Integer) {
    ArduinoController::addArduinoError(ArduinoError::CodeGetValueWithWrongType);
  }
  return _integer;
}

double Value::getDouble() const {
  if (_type != Type::Double) {
    ArduinoController::addArduinoError(ArduinoError::CodeGetValueWithWrongType);
  }
  return _double;
}

bool Value::getBoolean() const {
  if (_type != Type::Boolean) {
    ArduinoController::addArduinoError(ArduinoError::CodeGetValueWithWrongType);
  }
  return _boolean;
}
