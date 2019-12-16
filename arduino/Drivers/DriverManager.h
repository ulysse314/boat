#ifndef DriverManager_h
#define DriverManager_h

#include "Driver.h"
#include "List.h"

class DriverManager {
public:
  DriverManager();

  void addDriver(Driver *driver);
  void begin();
  void loop();

protected:
  List<Driver> _driverList;
};

#endif // DriverManager_h
