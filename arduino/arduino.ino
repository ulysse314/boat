#include "SensorList.h"
#include "GPSSensor.h"

SensorList sensorList;

void setup() {
  GPSSensor *gpsSensor = new GPSSensor();
  sensorList.addSensor(gpsSensor);
  sensorList.begin();
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
}
