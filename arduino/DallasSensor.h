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
  boolean                     _gotFirstValue;

public:
  static const char *sensorType(uint8_t address[8]);
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
  const char *sensorClass();
  const char *sensorType();
  const char *copyAddressString();
  boolean begin() { return this->loop(); };
  boolean isReady() { return _gotFirstValue; };
  boolean loop();
  boolean printAddress(Stream *serial);
  boolean printValues(Stream *serial);
};

#endif // DallasSensor_h