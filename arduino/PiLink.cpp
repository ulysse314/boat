#include "PiLink.h"

#include <Arduino.h>

#include "Controller.h"
#include "Error.h"
#include "Value.h"

#define TIMEOUT           4000

// static
PiLink *PiLink::getPiLink() {
  static PiLink *piLink = NULL;
  if (!piLink) {
    piLink = new PiLink(&Serial);
  }
  return piLink;
}

PiLink::PiLink(Stream *stream) :
    _stream(stream),
    _inputBuffer{0},
    _inputBufferLength(0),
    _nextTimeOut(millis() + TIMEOUT),
    _leftMotorController(NULL),
    _rightMotorController(NULL) {
}

void PiLink::outputController(const Controller *controller) {
  _stream->print("{\"name\":\"");
  _stream->print(controller->getName());
  _stream->print("\"");
  const List<Error>::Bucket *errorBucket = controller->getFirstErrorBucket();
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
  const List<Value>::Bucket *valueBucket = controller->getFirstValueBucket();
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
  return millis() - _nextTimeOut;
}

void PiLink::outputValue(const Value *value) {
  _stream->print("\"");
  _stream->print(value->getName());
  _stream->print("\":");
  if (!value || value->isNull()) {
    _stream->print("null");
  } else {
    switch (value->getType()) {
      case Value::Type::String:
        _stream->print("\"");
        _stream->print(value->getString());
        _stream->print("\"");
        break;
      case Value::Type::Integer:
        _stream->print(value->getInteger());
        break;
      case Value::Type::Double:
        _stream->print(value->getDouble());
        break;
      case Value::Type::Boolean:
        _stream->print(value->getBoolean() ? "true" : "false");
        break;
    }
  }
}

void PiLink::outputError(const Error *error) {
  _stream->print("[");
  _stream->print(error->getDomain());
  _stream->print(",");
  _stream->print(error->getCode());
  _stream->print(",");
  _stream->print(error->isPersistant() ? "true" : "false" );
  if (error->getMessage()) {
    _stream->print(",\"");
    _stream->print(error->getMessage());
    _stream->print("\"");
  }
  _stream->print("]");
}

void PiLink::processInputBuffer() {
  _inputBuffer[_inputBufferLength] = 0;
  size_t offset = 0;
  Controller *controller = NULL;
  if (strncmp(_inputBuffer, "lm ", strlen("lm ")) == 0) {
    offset = strlen("lm ");
    controller = _leftMotorController;
  } else if (strncmp(_inputBuffer, "rm ", strlen("rm ")) == 0) {
    offset = strlen("rm ");
    controller = _rightMotorController;
  }
  if (controller) {
    char *buffer = _inputBuffer + offset;
    controller->setValue(atoi(buffer));
  }
  _inputBufferLength = 0;
  _nextTimeOut = millis() + TIMEOUT;
}
