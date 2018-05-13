#include "ActuatorList.h"
#include "DallasSensor.h"
#include "GPSSensor.h"
#include "InfoActuatorSensor.h"
#include "MotorActuatorSensor.h"
#include "SensorList.h"
#include "OneWire.h"

#define ONE_WIRE_PIN          12

SensorList sensorList;
ActuatorList actuatorList;
OneWire oneWire(ONE_WIRE_PIN);
InfoActuatorSensor infoActuatorSensor;

void setup() {
  uint8_t address[8] = {0};
  GPSSensor *gpsSensor = new GPSSensor();
  sensorList.addSensor(gpsSensor);
  while (oneWire.find_address(address)) {
    if (DallasSensor::sensorType(address)) {
      sensorList.addSensor(new DallasSensor(address, &oneWire));
    }
  }
  MotorActuatorSensor *motorActuatorSensor = new MotorActuatorSensor();
  sensorList.addSensor(motorActuatorSensor);
  sensorList.addSensor(&infoActuatorSensor);

  sensorList.begin();
  sensorList.loop();
  actuatorList.addActuator(motorActuatorSensor);
  actuatorList.addActuator(&infoActuatorSensor);
  actuatorList.begin();
  actuatorList.loop();
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Ready");
  Serial.flush();
}

unsigned long lastPrint = 0;
unsigned long counter = 0;

void loop() {
  sensorList.loop();
  unsigned long currentTime = millis();
  unsigned long difference = currentTime - lastPrint;
  if (difference > 1000) {
    lastPrint = currentTime;
    sensorList.printInfo(&Serial, 0);
    Serial.print("-- ");
    Serial.print(difference);
    Serial.print(" ");
    Serial.println(counter);
    Serial.flush();
    sensorList.readValues();
    infoActuatorSensor.setCycleCount(counter);
    infoActuatorSensor.setLoopDuration(difference);
    counter = 0;
  }
  counter += 1;
  actuatorList.loop();
  actuatorList.listen(&Serial);
}
