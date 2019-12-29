#ifndef HardwareConfig_h
#define HardwareConfig_h

#include <inttypes.h>
#include <OneWire.h>

class TwoWire;
class OneWire;

class HardwareConfig {
public:
  HardwareConfig();
  ~HardwareConfig();

  void begin();

  TwoWire *getI2C() const { return _i2c; };
  OneWire *getOneWire() const { return _oneWire; };

  const OneWire::Address getLeftMotorDallasAddress() const;
  const OneWire::Address getRightMotorDallasAddress() const;
  const OneWire::Address getBatteryDallasAddress() const;
  const OneWire::Address getBalancerDallasAddress() const;

  uint8_t getPCA9685Address() const;
  uint8_t getINA219Address() const;
  uint8_t getADS1115Address() const;

  uint8_t getLeftMotorPWMID() const;
  uint8_t getRightMotorPWMID() const;

private:
  TwoWire *_i2c;
  OneWire *_oneWire;
};

#endif  // HardwareConfig_h
