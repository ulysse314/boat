#include "DriverManager.h"

DriverManager::DriverManager() :
    _driverList(List<Driver>::WhenDataRemoved::Free) {
}

void DriverManager::addDriver(Driver *driver) {
  _driverList.addData(driver);
}

void DriverManager::begin() {
  const List<Driver>::Bucket *bucket = _driverList.firstBucket();
  Driver *driver = NULL;
  while (_driverList.nextDataWithBucket(bucket, driver)) {
    driver->begin();
  }
}

void DriverManager::loop() {
  const List<Driver>::Bucket *bucket = _driverList.firstBucket();
  Driver *driver = NULL;
  while (_driverList.nextDataWithBucket(bucket, driver)) {
    if (driver->getAvailable()) {
      driver->loop();
    } else {
      driver->begin();
    }
  }
}
