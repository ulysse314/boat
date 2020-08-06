#ifndef BNO055Sensor_h
#define BNO055Sensor_h

#include "Sensor.h"

#include <inttypes.h>

class BNO055;
class TwoWire;

class BNO055Sensor : public Sensor {
public:
  class Vector {
  public:
    double x;
    double y;
    double z;
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

protected:
  BNO055 *_bno055;
  bool _available;
  unsigned long long _lastMeasure = 0;
  Vector _acc;
  Vector _mag;
  Vector _gyro;
  Vector _euler;
};

#endif // BNO055Sensor_h
