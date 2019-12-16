#include "HullController.h"

HullController::HullController(ADS1115Driver *ads1115Driver) :
    _ads1115Driver(ads1115Driver),
    _water(Value::Type::Integer, "water") {
}

HullController::~HullController() {
}

void HullController::begin() {
  addValue(&_water);
}

void HullController::sensorsHasBeenUpdated() {
  _water.setInteger(_ads1115Driver->getValue1());
}
