#include "AutoPilot.h"

AutoPilot::AutoPilot(GPSController *gpsController) :
  _mode(Mode::Disabled),
  _gpsController(gpsController) {
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
