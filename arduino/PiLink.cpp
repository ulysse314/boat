#include "PiLink.h"

#include <Arduino.h>

#include "Controller.h"
#include "Error.h"
#include "Value.h"

PiLink::PiLink(Stream *stream) :
    _stream(stream) {
}

void PiLink::outputController(const Controller *controller) {
  _stream->print("[\"");
  _stream->print(controller->getName());
  _stream->print("\",[");
  bool firstError = true;
  const List<Error>::Bucket *bucket = controller->getFirstErrorBucket();
  const Error *error = NULL;
  while (controller->nextErrorBucket(bucket, error)) {
    if (firstError) {
      firstError = false;
    } else {
      _stream->print(",");
    }
    outputError(error);
  }
  _stream->print("]");
  int valueCount = controller->getValueCount();
  for (int i = 0; i < valueCount; i++) {
    const Value *value = controller->getValueAtIndex(i);
    _stream->print(",");
    outputValue(value);
  }
  _stream->print("]\n\r");
}

void PiLink::outputValue(const Value *value) {
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