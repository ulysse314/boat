#ifndef GPSSensor_h
#define GPSSensor_h

#include "Sensor.h"

class MTK3339;

class GPSSensor : public Sensor {
public:
  virtual const char *sensorClass() const override { return "GPS"; };
  virtual const char *sensorType() const override { return "GPS"; };
  virtual void begin() override;
  virtual void loop() override;
  MTK3339 *getGPS() { return _mtk3339; };
  
protected:
    MTK3339 *_mtk3339;
};

#endif // GPSSensor_h
