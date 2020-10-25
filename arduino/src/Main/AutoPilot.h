#ifndef AutoPilot_h
#define AutoPilot_h

class GPSController;

class AutoPilot {
public:
  enum class Mode {
    Disabled,
    Standby,
  };

  AutoPilot(GPSController *gpsController);
  ~AutoPilot();

  Mode getActive() const { return _mode; };
  void loop();

private:
  Mode _mode;
  GPSController *_gpsController;
};

#endif  // AutoPilot_h
