#ifndef HullController_h
#define HullController_h

#include "ADS1115Driver.h"
#include "Controller.h"
#include "Value.h"

class HullController : public Controller {
public:
  HullController(ADS1115Driver *ads1115Driver);
  virtual ~HullController();

  const char *getName() const override { return "hull"; };
  void begin() override;
  void sensorsHasBeenUpdated() override;
  
protected:
  ADS1115Driver *_ads1115Driver;
  Value _water;
};

#endif // HullController_h
