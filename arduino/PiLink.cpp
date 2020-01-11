#include "PiLink.h"

#include <Arduino.h>

#include "ArduinoController.h"
#include "Controller.h"
#include "Error.h"
#include "HardwareConfig.h"
#include "MotorController.h"
#include "Value.h"

#define TIMEOUT           4000

static PiLink *sharedPiLink = NULL;

// static
PiLink *PiLink::generatePiLink(HardwareConfig *hardwareConfig) {
  if (!sharedPiLink) {
    sharedPiLink = new PiLink(hardwareConfig);
  }
  return sharedPiLink;
}

// static
PiLink *PiLink::getSharedPiLink() {
  return sharedPiLink;
}

PiLink::PiLink(HardwareConfig *hardwareConfig) :
    _stream(hardwareConfig->piSerial()),
    _inputBuffer{0},
    _inputBufferLength(0),
    _nextTimeOut(millis() + TIMEOUT),
    _leftMotorController(NULL),
    _rightMotorController(NULL) {
}

void PiLink::outputController(const Controller *controller) {
  _stream->print("{\"name\":\"");
  _stream->print(controller->name());
  _stream->print("\"");
  const List<Error>::Bucket *errorBucket = controller->firstErrorBucket();
  if (errorBucket) {
    _stream->print(",\"err\":[");
    bool firstError = true;
    const Error *error = NULL;
    while (controller->nextErrorBucket(errorBucket, error)) {
      if (firstError) {
        firstError = false;
      } else {
        _stream->print(",");
      }
      outputError(error);
      _stream->flush();
    }
    _stream->print("]");
  }
  _stream->flush();
  const List<Value>::Bucket *valueBucket = controller->firstValueBucket();
  const Value *value = NULL;
  while (controller->nextValueBucket(valueBucket, value)) {
    _stream->print(",");
    outputValue(value);
  _stream->flush();
  }
  _stream->print("}\n\r");
  _stream->flush();
}

void PiLink::listen() {
  while (_stream->available()) {
    char character = _stream->read();
    if (character == '\r' || character == '\n') {
      processInputBuffer();
      continue;
    }
    _inputBuffer[_inputBufferLength] = character;
    ++_inputBufferLength;
    if (_inputBufferLength == sizeof(_inputBuffer) -1) {
      processInputBuffer();
    }
  }
}

bool PiLink::hasTimedOut() {
  return (long int)(_nextTimeOut - millis()) < 0;
}

void PiLink::outputValue(const Value *value) {
  _stream->print("\"");
  _stream->print(value->name());
  _stream->print("\":");
  if (!value || value->isNull()) {
    _stream->print("null");
  } else {
    switch (value->type()) {
      case Value::Type::String:
        _stream->print("\"");
        _stream->print(value->valueAsString());
        _stream->print("\"");
        break;
      case Value::Type::Integer:
        _stream->print(value->valueAsInteger());
        break;
      case Value::Type::Double:
        _stream->print(value->valueAsDouble());
        break;
      case Value::Type::Boolean:
        _stream->print(value->valueAsBoolean() ? "true" : "false");
        break;
    }
  }
}

void PiLink::outputError(const Error *error) {
  _stream->print("[");
  _stream->print(error->getDomain());
  _stream->print(",");
  _stream->print(error->getCode());
  if (error->isPersistant() || error->getMessage()) {
    _stream->print(",");
    _stream->print(error->isPersistant() ? "true" : "false" );
  }
  if (error->getMessage()) {
    _stream->print(",\"");
    _stream->print(error->getMessage());
    _stream->print("\"");
  }
  _stream->print("]");
}

void PiLink::processInputBuffer() {
  _inputBuffer[_inputBufferLength] = 0;
  if (strncmp(_inputBuffer, "lm ", strlen("lm ")) == 0) {
    char *buffer = _inputBuffer + strlen("lm ");
    _leftMotorController->setValue(atoi(buffer));
  } else if (strncmp(_inputBuffer, "rm ", strlen("rm ")) == 0) {
    char *buffer = _inputBuffer + strlen("rm ");
    _rightMotorController->setValue(atoi(buffer));
  } else if (strncmp(_inputBuffer, "arduino ", strlen("arduino ")) == 0) {
    char *buffer = _inputBuffer + strlen("arduino ");
    _arduinoController->setCommand(buffer);
  } else if (strcmp(_inputBuffer, "ping") == 0) {
    // Nothing to do.
  } else {
    // Error
  }
  _inputBufferLength = 0;
  _nextTimeOut = millis() + TIMEOUT;
}
