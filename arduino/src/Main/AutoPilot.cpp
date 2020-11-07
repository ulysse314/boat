#include "AutoPilot.h"

AutoPilot::AutoPilot(GPSController *gpsController, HullController * hullController) :
  _mode(Mode::Disabled),
  _gpsController(gpsController),
  _hullController(hullController) {
}

AutoPilot::~AutoPilot() {
}

void AutoPilot::loop() {
  switch (_mode) {
  case Mode::Standby:
    break;
  case Mode::Disabled:
    break;
  }
}
