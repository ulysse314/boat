#include "ControllerManager.h"

#include <Arduino.h>

#include "../Controllers/Controller.h"
#include "../Main/PiLink.h"

ControllerManager::ControllerManager() : _controllerList(List<Controller>::WhenDataRemoved::Free) {}

ControllerManager::~ControllerManager() {}

void ControllerManager::addController(Controller *controller) {
  _controllerList.addData(controller);
}

void ControllerManager::addSensorsToList(SensorList *sensorList) {
  const List<Controller>::Bucket *bucket = _controllerList.firstBucket();
  Controller *controller = NULL;
  while (_controllerList.nextDataWithBucket(bucket, controller)) {
    controller->addSensorsToList(sensorList);
  }
}

void ControllerManager::begin() {
  const List<Controller>::Bucket *bucket = _controllerList.firstBucket();
  Controller *controller = NULL;
  while (_controllerList.nextDataWithBucket(bucket, controller)) {
    controller->begin();
  }
}

void ControllerManager::sensorsHasBeenUpdated() {
  const List<Controller>::Bucket *bucket = _controllerList.firstBucket();
  Controller *controller = NULL;
  while (_controllerList.nextDataWithBucket(bucket, controller)) {
    controller->clearNonPersistantErrors();
    controller->sensorsHasBeenUpdated();
  }
}

void ControllerManager::outputControllers(PiLink *piLink) {
  const List<Controller>::Bucket *bucket = _controllerList.firstBucket();
  Controller *controller = NULL;
  while (_controllerList.nextDataWithBucket(bucket, controller)) {
    piLink->outputController(controller);
  }
}
