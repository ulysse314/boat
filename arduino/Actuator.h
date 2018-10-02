#ifndef Actuator_h
#define Actuator_h

class Actuator {
public:
  virtual const char *addressString() const = 0;
  virtual bool begin() { return true; };
  virtual bool loop() { return true;};
  virtual bool processValues(const char *values) { return true; };
};

#endif // Actuator_h
