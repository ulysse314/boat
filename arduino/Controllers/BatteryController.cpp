#include "BatteryController.h"

#include "ADS1115Sensor.h"
#include "BatteryError.h"
#include "HardwareConfig.h"
#include "SensorList.h"
#include "Version.h"

#if IS_MOUSSAILLON
#define INA219ShuntValue 0.0015
#define INA219MaxCurrent 50.
#elif IS_TELEMAQUE
#define INA219ShuntValue 0.00125
#define INA219MaxCurrent 60.
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
                                     HardwareConfig *hardwareConfig) :
    _ads1115Sensor(ads1115Sensor),
    _ina219Sensor(INA219ShuntValue, INA219MaxCurrent, hardwareConfig->ina219Address(), hardwareConfig->i2c()),
    _batteryTemperatureSensor(hardwareConfig->batteryDallasAddress(), hardwareConfig->oneWire()),
    _balancerTemperatureSensor(hardwareConfig->balancerDallasAddress(), hardwareConfig->oneWire()),
    _voltage(Value::Type::Double, "volt"),
    _current(Value::Type::Double, "amp"),
    _batteryTemperature(Value::Type::Double, "batT"),
    _balancerTemperature(Value::Type::Double, "balT"),
    _balancer0(Value::Type::Integer, "bal0"),
    _balancer1(Value::Type::Integer, "bal1"),
    _balancer2(Value::Type::Integer, "bal2"),
    _currentConsumption(Value::Type::Double, "ah"),
    _lastCurrentValueTimestamp(0) {
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
  addValue(&_currentConsumption);
  _lastCurrentValueTimestamp = millis();
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
    unsigned long long currentMillis = millis();
    double consumption = _currentConsumption.valueAsDouble();
    double delta = current * ((double)(currentMillis - _lastCurrentValueTimestamp) / (double)1000. / (double)3600.);
    consumption += delta;
    _currentConsumption.setDouble(consumption);
    _lastCurrentValueTimestamp = currentMillis;
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

void BatteryController::setInitialCurrentConsumption(double initialConsumption) {
  initialConsumption += _currentConsumption.valueAsDouble();
  _currentConsumption.setDouble(initialConsumption);
}
