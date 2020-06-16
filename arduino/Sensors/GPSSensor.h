#ifndef GPSSensor_h
#define GPSSensor_h

#include "Sensor.h"

class MTK3339;
class Stream;

class GPSSensor : public Sensor {
public:
  GPSSensor(Stream *serial);
  ~GPSSensor();

  void switchToHighSpeed();
  void highSpeedSwitchDone();
  MTK3339 *mkt3339() { return _mtk3339; };

  virtual const char *sensorClass() const override { return "GPS"; };
  virtual const char *sensorType() const override { return "GPS"; };
  virtual void begin() override;
  virtual void loop() override;

protected:
    Stream *const _serial;
    MTK3339 *const _mtk3339;
};

#endif // GPSSensor_h
