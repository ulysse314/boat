class Actuator {
public:
  virtual const char *getAddress() const = 0;
  virtual void begin();
  virtual void loop();
  virtual bool processValues(const char *values);
};
