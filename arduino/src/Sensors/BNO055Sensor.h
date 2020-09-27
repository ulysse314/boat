#ifndef BNO055Sensor_h
#define BNO055Sensor_h

#include "../Sensors/Sensor.h"

#include <inttypes.h>

class BNO055;
class TwoWire;

#define MAG_MAX_COUNT 10

class BNO055Sensor : public Sensor {
public:
  class Vector {
  public:
    double x;
    double y;
    double z;
  };
  enum class SystemError {
    NoError,
    PeripheralInitializationError,
    SystemInitializationError,
    SelfTestResultFailed,
    RegisterMapValueOutOfRange,
    RegisterMapAddressOutOfRange,
    RegisterMapWriteError,
    LowPowerNotAvailableForSelectedOperationMode,
    AccelerometerPowerModeNotAvailable,
    FusionAlgorithmConfigurationError,
    SensorConfigurationError,
  };
  enum class SystemStatus {
    Idle,
    Error,
    InitializingPeripherals,
    SystemInitialization,
    ExecutingSelfTest,
    RunningWithFusionAlgorithm,
    RunningWihtoutFusionAlgorithm,
  };
  enum class Calibration {
    None = 0,
    Low = 1,
    Medium = 2,
    Fully = 3,
  };

  BNO055Sensor(uint8_t address, TwoWire *i2cBus);
  ~BNO055Sensor();

  // Sensor
  virtual const char *sensorClass() const override { return "ADS"; };
  virtual const char *sensorType() const override { return "BNO055"; };
  virtual void begin() override;
  virtual void loop() override;
  virtual bool readValues() override;

  bool getAvailable() const { return _available; };
  Vector getAcc() const { return _acc; };
  Vector getMag() const { return _mag; };
  Vector getGyro() const { return _gyro; };
  Vector getEuler() const { return _euler; };
  double headingForVector(Vector vector) const;
  double getHeading() const { return headingForVector(_mag); };
  double getAverageHeading() const;
  SystemError getSystemError() const { return _systemError; };
  SystemStatus getSystemStatus() const { return _systemStatus; };
  Calibration getAccelCalibration() const { return _accelCalibration; };
  Calibration getGyroCalibration() const { return _gyroCalibration; };
  Calibration getMagCalibration() const { return _magCalibration; };
  Calibration getSysCalibration() const { return _sysCalibration; };
  bool getAccelSelfTest() const { return _accelSelfTest; };
  bool getGyroSelfTest() const { return _gyroSelfTest; };
  bool getMagSelfTest() const { return _magSelfTest; };
  bool getSysSelfTest() const { return _sysSelfTest; };

protected:
  BNO055 *_bno055;
  bool _available;
  unsigned long long _lastMeasure = 0;
  Vector _acc;
  Vector _mag;
  Vector _gyro;
  Vector _euler;
  Vector _magValues[MAG_MAX_COUNT] = { { 0, 0, 0 } };
  Calibration _accelCalibration, _gyroCalibration, _magCalibration, _sysCalibration;
  bool _accelSelfTest, _magSelfTest, _gyroSelfTest, _sysSelfTest;
  size_t _magValueIndex = 0;
  SystemError _systemError = SystemError::NoError;
  SystemStatus _systemStatus = SystemStatus::Idle;
  unsigned long long _lastDataFetch;
};

#endif // BNO055Sensor_h
