#ifndef Actuator_h
#define Actuator_h

class Actuator {
public:
  virtual ~Actuator() {};
  virtual const char *addressString() const = 0;
  virtual void begin() {};
  virtual void loop() {};
  virtual bool processValues(const char *) { return true; };
};

#endif // Actuator_h
