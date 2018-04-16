#include "ActuatorList.h"
#include "GPSSensor.h"
#include "SensorList.h"
#include "OneWire.h"
#include "DallasSensor.h"

#define ONE_WIRE_PIN          13

SensorList sensorList;
ActuatorList activatorList;
OneWire oneWire(ONE_WIRE_PIN);

void setup() {
  uint8_t address[8] = {0};
  GPSSensor *gpsSensor = new GPSSensor();
  sensorList.addSensor(gpsSensor);
  while (oneWire.find_address(address)) {
    if (DallasSensor::sensorType(address)) {
      sensorList.addSensor(new DallasSensor(address, &oneWire));
    }
  }
  sensorList.begin();
  sensorList.loop();
  activatorList.begin();
  activatorList.loop();
  while (!Serial);
  Serial.begin(115200);
}

unsigned long lastPrint = 0;

void loop() {
  sensorList.loop();
  unsigned long currentTime = millis();
  if (currentTime - lastPrint> 1000) {
    lastPrint = currentTime;
    sensorList.printInfo(&Serial, 0);
    Serial.println("--");
  }
  activatorList.loop();
  activatorList.listen(&Serial);
}
