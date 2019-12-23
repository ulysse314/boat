#include "BatteryController.h"

#include "ADS1115Sensor.h"
#include "BatteryError.h"
#include "SensorList.h"
#include "Version.h"

#if IS_MOUSSAILLON
#define INA219ShuntValue 0.0015
#define INA219MaxCurrent 50.
const uint8_t kBalancerDallasAddress[8] = { 0x28, 0xAA, 0x73, 0x4E, 0x55, 0x14, 0x01, 0x9C };
const uint8_t kBatteryDallasAddress[8] = { 0x28, 0x66, 0x05, 0x1F, 0x03, 0x00, 0x00, 0xFB };
#elif IS_TELEMAQUE
#define INA219ShuntValue 0.00125
#define INA219MaxCurrent 60.
const uint8_t kBalancerDallasAddress[8] = { 0x28, 0xAA, 0x73, 0x4E, 0x55, 0x14, 0x01, 0x9C };
const uint8_t kBatteryDallasAddress[8] = { 0x28, 0x58, 0xDB, 0x1E, 0x03, 0x00, 0x00, 0x76 };
#else
#error *** No boat defined ***
#endif

#define kInfoVoltage                     12.4
#define kWarningVoltage                  12.
#define kCriticalVoltage                 11.6
#define kInfoTemperature                 50.0
#define kWarningTemperature              60.0
#define kCriticalTemperature             70.0

BatteryController::BatteryController(ADS1115Sensor *ads1115Sensor,
                                     uint8_t ina219Address,
                                     TwoWire *i2c,
                                     OneWire *oneWire) :
    _ads1115Sensor(ads1115Sensor),
    _ina219Sensor(INA219ShuntValue, ina219Address, ina219Address, i2c),
    _batteryTemperatureSensor(kBatteryDallasAddress, oneWire),
    _balancerTemperatureSensor(kBalancerDallasAddress, oneWire),
    _voltage(Value::Type::Double, "volt"),
    _current(Value::Type::Double, "amp"),
    _batteryTemperature(Value::Type::Double, "batT"),
    _balancerTemperature(Value::Type::Double, "balT"),
    _balancer0(Value::Type::Integer, "bal0"),
    _balancer1(Value::Type::Integer, "bal1"),
    _balancer2(Value::Type::Integer, "bal2") {
}

BatteryController::~BatteryController() {
}

void BatteryController::begin() {
  addValue(&_voltage);
  addValue(&_current);
  addValue(&_batteryTemperature);
  addValue(&_balancerTemperature);
  addValue(&_balancer0);
  addValue(&_balancer1);
  addValue(&_balancer2);
}

void BatteryController::addSensorsToList(SensorList *sensorList) {
  sensorList->addSensor(&_ina219Sensor);
  sensorList->addSensor(&_batteryTemperatureSensor);
  sensorList->addSensor(&_balancerTemperatureSensor);
}

void BatteryController::sensorsHasBeenUpdated() {
  if (_ina219Sensor.hasValue()) {
    double voltage = _ina219Sensor.getVoltage();
    _voltage.setDouble(voltage);
    if (voltage > kInfoVoltage) {
      // No error.
    } else if (voltage > kWarningVoltage) {
      addError(new BatteryError(BatteryError::CodeVoltageInfo));
    } else if (voltage > kCriticalVoltage) {
      addError(new BatteryError(BatteryError::CodeVoltageWarning));
    } else {
      addError(new BatteryError(BatteryError::CodeVoltageCritical));
    }
    double current = _ina219Sensor.getCurrent();
    _current.setDouble(current);
  } else {
    addError(new BatteryError(BatteryError::CodeINA219NotFound));
    _voltage.setNull();
    _current.setNull();
  }
  if (_batteryTemperatureSensor.hasValue()) {
    double temperature = _batteryTemperatureSensor.celsius();
    _batteryTemperature.setDouble(temperature);
    if (temperature < kInfoTemperature) {
      // No error.
    } else if (temperature < kWarningTemperature) {
      addError(new BatteryError(BatteryError::CodeBatteryTemperatureInfo));
    } else if (temperature < kCriticalTemperature) {
      addError(new BatteryError(BatteryError::CodeBatteryTemperatureWarning));
    } else {
      addError(new BatteryError(BatteryError::CodeBatteryTemperatureCritical));
    }
  } else {
    addError(new BatteryError(BatteryError::CodeBatteryTemperatureUnknown));
    _batteryTemperature.setNull();
  }
  if (_balancerTemperatureSensor.hasValue()) {
    double temperature = _balancerTemperatureSensor.celsius();
    _balancerTemperature.setDouble(temperature);
    if (temperature < kInfoTemperature) {
      // No error.
    } else if (temperature < kWarningTemperature) {
      addError(new BatteryError(BatteryError::CodeBalancerTemperatureInfo));
    } else if (temperature < kCriticalTemperature) {
      addError(new BatteryError(BatteryError::CodeBalancerTemperatureWarning));
    } else {
      addError(new BatteryError(BatteryError::CodeBalancerTemperatureCritical));
    }
  } else {
    addError(new BatteryError(BatteryError::CodeBalancerTemperatureUnknown));
    _balancerTemperature.setNull();
  }
  if (_ads1115Sensor->getAvailable()) {
    _balancer0.setInteger(_ads1115Sensor->getValue1());
    _balancer1.setInteger(_ads1115Sensor->getValue2());
    _balancer2.setInteger(_ads1115Sensor->getValue3());
  } else {
    addError(new BatteryError(BatteryError::CodeADS1115NotFound));
    _balancer0.setNull();
    _balancer1.setNull();
    _balancer2.setNull();
  }
}
