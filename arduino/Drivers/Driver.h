#ifndef Driver_h
#define Driver_h

class Driver {
public:
  virtual void begin() = 0;
  virtual void loop() = 0;
  virtual bool getAvailable() { return _available; };

protected:
  bool _available = false;
};

#endif // Driver_h
