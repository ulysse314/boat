#include "ActuatorList.h"
#include "ArduinoController.h"
#include "ControllerManager.h"
#include "DallasSensor.h"
#include "GPSController.h"
#include "GPSSensor.h"
#include "InfoActuatorSensor.h"
#include "MemoryFree.h"
#include "MotorActuatorSensor.h"
#include "PiLink.h"
#include "PowerSensor.h"
#include "SensorList.h"
#include "OneWire.h"

#define ONE_WIRE_PIN          12

SensorList *sensorList = NULL;
ActuatorList *actuatorList = NULL;
OneWire *oneWire = NULL;
InfoActuatorSensor *infoActuatorSensor = NULL;

ControllerManager *controllerManager = NULL;
ArduinoController *arduinoController = NULL;
GPSController *gpsController = NULL;

PiLink *piLink = NULL;

void initGlobal() {
  sensorList = new SensorList();
  actuatorList = new ActuatorList();
  oneWire = new OneWire(ONE_WIRE_PIN);
  infoActuatorSensor = new InfoActuatorSensor();
  controllerManager = new ControllerManager();

  gpsController = new GPSController();
  controllerManager->addController(gpsController);
  arduinoController = ArduinoController::getArduinoController();
  controllerManager->addController(arduinoController);
  controllerManager->addSensorsToList(sensorList);

  piLink = new PiLink(&Serial);
    Serial.println(__LINE__);

  uint8_t address[8] = {0};
    Serial.println(__LINE__);
  while (oneWire->find_address(address)) {
    if (DallasSensor::sensorType(address)) {
      sensorList->addSensor(new DallasSensor(address, oneWire));
    }
  }
    sensorList->addSensor(new PowerSensor(A0, A1));
//  MotorActuatorSensor *motorActuatorSensor = new MotorActuatorSensor();
//  sensorList->addSensor(motorActuatorSensor);
  sensorList->addSensor(infoActuatorSensor);

  sensorList->begin();
  controllerManager->begin();
  sensorList->loop();
//  actuatorList->addActuator(motorActuatorSensor);
  actuatorList->addActuator(infoActuatorSensor);
  actuatorList->begin();
  actuatorList->loop();
}

void setup() {
  analogReadResolution(12);
  pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.begin(115200);
  for (int i = 0; i < 4; i++) {
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(500);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(500);                       // wait for a second
    Serial.println(i);
  }
  Serial.println("Starting...");
  initGlobal();
  Serial.println("Started!");
}

unsigned long lastPrint = 0;
unsigned long counter = 0;

void loop() {
  sensorList->loop();
  unsigned long currentTime = millis();
  unsigned long difference = currentTime - lastPrint;
  if (difference > 1000) {
    digitalWrite(LED_BUILTIN, HIGH);
    lastPrint = currentTime;

    arduinoController->setCycleCount(counter);
    arduinoController->setLoopDuration(difference);
    controllerManager->sensorsHasBeenUpdated();
    controllerManager->outputControllers(piLink);
    sensorList->readValues();
    arduinoController->setComputeTime(millis() - currentTime);
#if 0
    sensorList->printInfo(&Serial, 0);
    Serial.print("-- ");
    Serial.print(difference);
    Serial.print(" ");
    Serial.println(counter);
    controllerManager->sensorsHasBeenUpdated();
    Serial.flush();
    sensorList->readValues();
    infoActuatorSensor->setCycleCount(counter);
    infoActuatorSensor->setLoopDuration(difference);
    actuatorList->loop();
    actuatorList->listen(&Serial);
#endif
    counter = 0;
    digitalWrite(LED_BUILTIN, LOW);
  }
  ++counter;
}
