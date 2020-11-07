#ifndef AutoPilot_h
#define AutoPilot_h

class GPSController;
class HullController;

class AutoPilot {
public:
  enum class Mode {
    Disabled,
    Standby,
  };

  AutoPilot(GPSController *gpsController, HullController *hullController);
  ~AutoPilot();

  Mode getActive() const { return _mode; };
  void loop();

private:
  Mode _mode;
  GPSController *_gpsController;
  HullController *_hullController;
};

#endif  // AutoPilot_h
