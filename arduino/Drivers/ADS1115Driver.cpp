#include "ADS1115Driver.h"

#define MEASURE_EVERY      100

ADS1115Driver::ADS1115Driver() :
    _ads1115() {
}

void ADS1115Driver::begin() {
  _available = _ads1115.begin();
}

void ADS1115Driver::loop() {
  if ((unsigned long long)(millis() - _lastMeasure) > MEASURE_EVERY) {
    _lastMeasure = millis();
    _value0 = _ads1115.readADC_SingleEnded(0);
    _value1 = _ads1115.readADC_SingleEnded(1);
    _value2 = _ads1115.readADC_SingleEnded(2);
    _value3 = _ads1115.readADC_SingleEnded(3);
  }
}
