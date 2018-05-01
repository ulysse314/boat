#ifndef DallasSensor_h
#define DallasSensor_h

#include "OneWire.h"
#include "Sensor.h"

typedef enum {
  DallasSensorStateReset,
  DallasSensorStateResetWait,
} DallasSensorState;

typedef enum {
  DallasSensorTypeDS18S20 = 0x10,
  DallasSensorTypeDS18B20 = 0x28,
  DallasSensorTypeDS1822 = 0x22,
} DallasSensorType;

class OneWire;

class DallasSensor : public Sensor
{
  OneWire                     *_oneWire;
  uint8_t                     _address[8];
  DallasSensorState           _state;
  unsigned long               _timer;
  float                       _celsius;
  bool                        _gotFirstValue;

public:
  static const char *sensorType(const uint8_t address[8]);
  inline static int maxConversionTimeMillisForBits(int bits) {
    if (bits == 9) return 94;
    if (bits == 10) return 188;
    if (bits == 11) return 375;
    return 750;
  };

  DallasSensor(uint8_t adresse[8], OneWire *oneWire);
  
  void sendConvertCommand();
  void sendReadCommand();
  
  float celsius() { return _celsius; };
  
  // sensor
  const char *sensorClass() const override { return "DALLAS"; };
  const char *sensorType() const override;
  const char *copyAddressString() const override;
  bool begin() override { return this->loop(); };
  bool isReady() override { return _gotFirstValue; };
  bool loop() override;
  bool printAddress(Stream *serial) override;
  bool printValues(Stream *serial) override;
};

#endif // DallasSensor_h
