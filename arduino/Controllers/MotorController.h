#ifndef MotorController_h
#define MotorController_h

#include "Controller.h"
#include "Value.h"

#include <OneWire.h>

class DallasSensor;
class HardwareConfig;
class PWMDriver;

class MotorController : public Controller {
public:
  static MotorController *LeftMotor(PWMDriver *pwmDriver, HardwareConfig *hardwareConfig);
  static MotorController *RightMotor(PWMDriver *pwmDriver, HardwareConfig *hardwareConfig);

  ~MotorController();

  // Controller
  const char *name() const override { return _name; };
  void addSensorsToList(SensorList *sensorList) override;
  void sensorsHasBeenUpdated() override;
  void begin() override;

  void setValue(int);

protected:
  MotorController(const char *name, const char *humanName, PWMDriver *pwmDriver, uint8_t motorID, const OneWire::Address dallasAddress, HardwareConfig *hardwareConfig);

  const char *const _name;
  const char *const _humanName;
  PWMDriver *const _pwmDriver;
  DallasSensor *const _temperatureSensor;
  uint8_t _motorID;
  Value _power;
  Value _temperature;
};

#endif // MotorController_h
