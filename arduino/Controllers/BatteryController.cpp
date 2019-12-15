#include "BatteryController.h"

#include "BatteryError.h"
#include "SensorList.h"
#include "Version.h"

#if IS_MOUSSAILLON
#define INA219ShuntValue 0.0015
#define INA219MaxCurrent 50.
const uint8_t kDallasAddress[8] = { 0x28, 0x66, 0x05, 0x1F, 0x03, 0x00, 0x00, 0xFB };
#elif IS_TELEMAQUE
#define INA219ShuntValue 0.00125
#define INA219MaxCurrent 60.
const uint8_t kDallasAddress[8] = { 0x28, 0x58, 0xDB, 0x1E, 0x03, 0x00, 0x00, 0x76 };
#else
#error *** No boat defined ***
#endif

#define kINA219Address 0x41
#define kInfoTemperature                 50.0
#define kWarningTemperature              60.0
#define kCriticalTemperature             70.0

BatteryController::BatteryController(TwoWire *i2c, OneWire *oneWire) :
    _ina219Sensor(i2c, kINA219Address, INA219ShuntValue, INA219MaxCurrent),
    _temperatureSensor(kDallasAddress, oneWire),
    _voltage(Value::Type::Double, "volt"),
    _current(Value::Type::Double, "amp"),
    _temperature(Value::Type::Double, "temp") {
}

BatteryController::~BatteryController() {
}

void BatteryController::begin() {
  addValue(&_voltage);
  addValue(&_current);
  addValue(&_temperature);
}

void BatteryController::addSensorsToList(SensorList *sensorList) {
  sensorList->addSensor(&_ina219Sensor);
  sensorList->addSensor(&_temperatureSensor);
}

void BatteryController::sensorsHasBeenUpdated() {
  if (_ina219Sensor.hasValue()) {
    double voltage = _ina219Sensor.getVoltage();
    _voltage.setDouble(voltage);
    double current = _ina219Sensor.getCurrent();
    _current.setDouble(current);
  } else {
    addError(new BatteryError(BatteryError::CodeINA219NotFound));
    _voltage.setNull();
    _current.setNull();
  }
  if (_temperatureSensor.hasValue()) {
    double temperature = _temperatureSensor.celsius();
    _temperature.setDouble(temperature);
    if (temperature < kInfoTemperature) {
      // No error.
    } else if (temperature < kWarningTemperature) {
      addError(new BatteryError(BatteryError::CodeTemperatureInfo));
    } else if (temperature < kCriticalTemperature) {
      addError(new BatteryError(BatteryError::CodeTemperatureWarning));
    } else {
      addError(new BatteryError(BatteryError::CodeTemperatureCritical));
    }
  } else {
    addError(new BatteryError(BatteryError::CodeTemperatureUnknown));
    _temperature.setNull();
  }
}
