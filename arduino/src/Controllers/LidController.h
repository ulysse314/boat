#ifndef LidController_h
#define LidController_h

#include "../Controllers/Controller.h"
#include "../Errors/ArduinoError.h"

class HardwareConfig;
class I2CDevice;

class LidController : public Controller {
public:
  LidController(HardwareConfig *hardwareConfig);
  ~LidController();

  // Controller
  const char *name() const override { return "lid"; };
  void begin() override;

  bool playLight(size_t lightIndex);
  bool stopLight();
  bool playSong(size_t songIndex);
  bool stopSong();

protected:
  bool sendPlayCommand(String command, size_t index);
  bool sendCommand(String command);

  I2CDevice *_i2cDevice;
};

#endif // LidController_h
