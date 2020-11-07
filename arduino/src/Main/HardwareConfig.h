#ifndef HardwareConfig_h
#define HardwareConfig_h

#include <inttypes.h>
#include <OneWire.h>

class Serial_;
class Stream;
class TwoWire;
class Uart;

class HardwareConfig {
public:
  HardwareConfig();
  ~HardwareConfig();

  void begin();

  TwoWire *i2c() const { return _i2c; };
  OneWire *oneWire() const { return _oneWire; };
  Stream *piSerial() const;
  Stream *gpsSerial() const;

  void switchGPSSerialToHighSpeed();

  const OneWire::Address leftMotorDallasAddress() const;
  const OneWire::Address rightMotorDallasAddress() const;
  const OneWire::Address batteryDallasAddress() const;
  const OneWire::Address balancerDallasAddress() const;

  uint8_t pca9685Address() const;
  uint8_t ina219Address() const;
  uint8_t ads1115Address() const;
  uint8_t bme680Address() const;
  uint8_t bno055Address() const;
  uint8_t lidAddress() const;

  uint8_t leftMotorPWMID() const;
  uint8_t rightMotorPWMID() const;

  uint8_t ledPin() const;
  uint8_t leakPin() const;

private:
  TwoWire *const _i2c;
  OneWire *const _oneWire;
  Serial_ *const _piSerial;
  Uart *const _gpsSerial;
};

#endif  // HardwareConfig_h
