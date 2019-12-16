#ifndef Driver_h
#define Driver_h

class Driver {
public:
  virtual void begin() = 0;
  virtual void loop() = 0;
};

#endif // Driver_h
