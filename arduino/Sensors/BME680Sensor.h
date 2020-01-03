#ifndef BME680Sensor_h
#define BME680Sensor_h

#include "Sensor.h"

#include <BME680.h>

class BME680Sensor : public Sensor {
public:
  BME680Sensor(uint8_t address, TwoWire *i2cBus);

  virtual const char *sensorClass() const override { return "BME"; };
  virtual const char *sensorType() const override { return "BME680"; };
  virtual void begin() override;
  virtual void loop() override;
  virtual bool readValues() override;

  bool getAvailable() const { return _available; };
  float getTemperature() const { return _temperature; };
  float getPressure() const { return _pressure; };
  float getHumidity() const { return _humidity; };

protected:
  BME680 _bme680;
  bool _available;
  bool _waitingForData;
  float _temperature;
  float _humidity;
  float _pressure;
};

#endif // BME680Sensor_h
