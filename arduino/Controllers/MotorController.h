#ifndef MotorController_h
#define MotorController_h

#include "Controller.h"
#include "Value.h"

class DallasSensor;
class HardwareConfig;
class PWMDriver;

class MotorController : public Controller {
public:
  static MotorController *LeftMotor(PWMDriver *pwmDriver, HardwareConfig *hardwareConfig);
  static MotorController *RightMotor(PWMDriver *pwmDriver, HardwareConfig *hardwareConfig);

  ~MotorController();

  // Controller
  const char *getName() const override { return _name; };
  void addSensorsToList(SensorList *sensorList) override;
  void sensorsHasBeenUpdated() override;
  void begin() override;

  void setValue(int);
  
protected:
  MotorController(const char *name, PWMDriver *pwmDriver, uint8_t motorID, const uint8_t dallasAddress[8], HardwareConfig *hardwareConfig);

  const char *_name;
  PWMDriver *_pwmDriver;
  DallasSensor *_temperatureSensor;
  uint8_t _motorID;
  Value _power;
  Value _temperature;
};

#endif // MotorController_h
