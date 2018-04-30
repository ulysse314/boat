#include "ActuatorList.h"
#include "DallasSensor.h"
#include "GPSSensor.h"
#include "MotorActuator.h"
#include "SensorList.h"
#include "OneWire.h"

#define ONE_WIRE_PIN          13

SensorList sensorList;
ActuatorList actuatorList;
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
  MotorActuator *motorActuator = new MotorActuator();
  actuatorList.addActuator(motorActuator);
  actuatorList.begin();
  actuatorList.loop();
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Ready");
  Serial.flush();
}

unsigned long lastPrint = 0;

void loop() {
  sensorList.loop();
  unsigned long currentTime = millis();
  unsigned long difference = currentTime - lastPrint;
  if (difference > 1000) {
    lastPrint = currentTime;
    sensorList.printInfo(&Serial, 0);
    Serial.print("--");
    Serial.println(difference);
    Serial.flush();
  }
  actuatorList.loop();
  actuatorList.listen(&Serial);
}
