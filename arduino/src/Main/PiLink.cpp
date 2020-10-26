#include "PiLink.h"

#include "../Controllers/ArduinoController.h"
#include "../Controllers/BatteryController.h"
#include "../Controllers/Controller.h"
#include "../Controllers/LidController.h"
#include "../Controllers/MotorController.h"
#include "../Controllers/Value.h"
#include "../Errors/Error.h"
#include "../Main/AutoPilot.h"
#include "../Main/HardwareConfig.h"

#include <Arduino.h>

#define PI_TIMEOUT             4000
#define RESET_COMSUMPTION_CMD  "resetcurrentconsumption"
#define PING_CMD               "ping"
#define LEFT_MOTOR_CMD         "lm "
#define RIGHT_MOTOR_CMD        "rm "
#define ARDUINO_CMD            "arduino "
#define PLAY_LIGHT_CMD         "light "
#define PLAY_SONG_CMD          "song "
#define STOP_LIGHT             "stop light"
#define STOP_SONG              "stop song"
#define RESTART_CMD            "restart"

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
    _nextTimeOut(millis() + PI_TIMEOUT),
    _arduinoController(NULL),
    _batteryController(NULL),
    _lidController(NULL),
    _leftMotorController(NULL),
    _rightMotorController(NULL),
    _autoPilot(NULL) {
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
  if (strncmp(_inputBuffer, LEFT_MOTOR_CMD, strlen(LEFT_MOTOR_CMD)) == 0) {
    char *buffer = _inputBuffer + strlen(LEFT_MOTOR_CMD);
    _leftMotorController->setValue(atoi(buffer));
  } else if (strncmp(_inputBuffer, RIGHT_MOTOR_CMD, strlen(RIGHT_MOTOR_CMD)) == 0) {
    char *buffer = _inputBuffer + strlen(RIGHT_MOTOR_CMD);
    _rightMotorController->setValue(atoi(buffer));
  } else if (strncmp(_inputBuffer, ARDUINO_CMD, strlen(ARDUINO_CMD)) == 0) {
    char *buffer = _inputBuffer + strlen(ARDUINO_CMD);
    _arduinoController->setCommand(buffer);
  } else if (strcmp(_inputBuffer, RESET_COMSUMPTION_CMD) == 0) {
    _batteryController->resetCurrentConsumption();
  } else if (strncmp(_inputBuffer, PLAY_LIGHT_CMD, strlen(PLAY_LIGHT_CMD)) == 0) {
    char *buffer = _inputBuffer + strlen(PLAY_LIGHT_CMD);
    _lidController->playLight(atoi(buffer));
  } else if (strcmp(_inputBuffer, STOP_LIGHT) == 0) {
    _lidController->stopLight();
  } else if (strncmp(_inputBuffer, PLAY_SONG_CMD, strlen(PLAY_SONG_CMD)) == 0) {
    char *buffer = _inputBuffer + strlen(PLAY_SONG_CMD);
    _lidController->playSong(atoi(buffer));
  } else if (strcmp(_inputBuffer, STOP_SONG) == 0) {
    _lidController->stopSong();
  } else if (strcmp(_inputBuffer, RESTART_CMD) == 0) {
    _arduinoController->restart();
  } else if (strcmp(_inputBuffer, PING_CMD) == 0) {
    // Nothing to do.
  } else {
    // Error
  }
  _inputBufferLength = 0;
  _nextTimeOut = millis() + PI_TIMEOUT;
}
