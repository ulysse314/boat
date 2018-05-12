#ifndef Actuator_h
#define Actuator_h

class Actuator {
public:
  virtual const char *addressString() const = 0;
  virtual bool begin() {};
  virtual bool loop() {};
  virtual bool processValues(const char *values) { return true; };
};

#endif // Actuator_h
