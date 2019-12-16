#include "HullController.h"

#include "HullError.h"

#define kLeakThresshold      3000

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
  if (_ads1115Driver->getAvailable()) {
    uint16_t water = _ads1115Driver->getValue1();
    _water.setInteger(water);
    if (water < kLeakThresshold) {
      addError(new HullError(HullError::CodeLeak));
    }
  } else {
    _water.setNull();
    addError(new HullError(HullError::CodeADS1115NotFound));
  }
}
