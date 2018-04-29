class Actuator {
public:
  virtual const char *getAddress() const = 0;
  virtual bool begin() {};
  virtual bool loop() {};
  virtual bool processValues(const char *values) {};
};
