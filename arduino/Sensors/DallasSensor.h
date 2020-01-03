#ifndef DallasSensor_h
#define DallasSensor_h

#include "Sensor.h"

#include <OneWire.h>

typedef enum {
  DallasSensorTypeDS18S20 = 0x10,
  DallasSensorTypeDS18B20 = 0x28,
  DallasSensorTypeDS1822 = 0x22,
} DallasSensorType;

class OneWire;

class DallasSensor : public Sensor
{
public:
  static const char *sensorType(const OneWire::Address address);
  inline static int maxConversionTimeMillisForBits(int bits) {
    if (bits == 9) return 94;
    if (bits == 10) return 188;
    if (bits == 11) return 375;
    return 750;
  };

  DallasSensor(const OneWire::Address adress, OneWire *oneWire);
  
  float celsius() const { return _celsius; };
  bool hasValue() const { return _hasValue; };
  
  // sensor
  const char *sensorClass() const override { return "DALLAS"; };
  const char *sensorType() const override;
  const char *copyAddressString() const override;
  bool readValues() override;
  void loop() override;

private:
  void sendConvertCommand();
  void sendReadCommand();

  OneWire *_oneWire;
  OneWire::Address _address;
  bool _hasValue;
  float _celsius;
  unsigned long _timer;
  bool _tried;
};

#endif // DallasSensor_h
