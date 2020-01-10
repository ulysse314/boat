#ifndef HardwareConfig_h
#define HardwareConfig_h

#include <inttypes.h>
#include <OneWire.h>

class Serial_;
class Stream;
class TwoWire;

class HardwareConfig {
public:
  HardwareConfig();
  ~HardwareConfig();

  void begin();

  TwoWire *i2c() const { return _i2c; };
  OneWire *oneWire() const { return _oneWire; };
  Stream *piSerial() const;

  const OneWire::Address leftMotorDallasAddress() const;
  const OneWire::Address rightMotorDallasAddress() const;
  const OneWire::Address batteryDallasAddress() const;
  const OneWire::Address balancerDallasAddress() const;

  uint8_t pca9685Address() const;
  uint8_t ina219Address() const;
  uint8_t ads1115Address() const;
  uint8_t bme680Address() const;

  uint8_t leftMotorPWMID() const;
  uint8_t rightMotorPWMID() const;

  uint8_t ledPin() const;

private:
  TwoWire *const _i2c;
  OneWire *const _oneWire;
  Serial_ *const _serial;
};

#endif  // HardwareConfig_h
