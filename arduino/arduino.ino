#include "ArduinoController.h"
#include "ControllerManager.h"
#include "DallasSensor.h"
#include "GPSController.h"
#include "GPSSensor.h"
#include "InfoActuatorSensor.h"
#include "MemoryFree.h"
#include "MotorActuatorSensor.h"
#include "MotorController.h"
#include "PiLink.h"
#include "PowerSensor.h"
#include "SensorList.h"
#include "OneWire.h"
#include "Adafruit_PWMServoDriver.h"

#define ONE_WIRE_PIN          12

SensorList *sensorList = NULL;
OneWire *oneWire = NULL;
InfoActuatorSensor *infoActuatorSensor = NULL;

ControllerManager *controllerManager = NULL;
ArduinoController *arduinoController = NULL;
GPSController *gpsController = NULL;
MotorController *leftMotorController = NULL;
MotorController *rightMotorController = NULL;
Adafruit_PWMServoDriver *pwmDriver = NULL;

PiLink *piLink = NULL;

void initGlobal() {
  sensorList = new SensorList();
  oneWire = new OneWire(ONE_WIRE_PIN);
  infoActuatorSensor = new InfoActuatorSensor();
  controllerManager = new ControllerManager();
  pwmDriver = new Adafruit_PWMServoDriver(41);
  piLink = new PiLink(&Serial);

  gpsController = new GPSController();
  controllerManager->addController(gpsController);
  arduinoController = ArduinoController::getArduinoController();
  controllerManager->addController(arduinoController);
  leftMotorController = MotorController::LeftMotor(oneWire, pwmDriver);
  controllerManager->addController(leftMotorController);
  rightMotorController = MotorController::RightMotor(oneWire, pwmDriver);
  controllerManager->addController(rightMotorController);

  controllerManager->addSensorsToList(sensorList);
  piLink->setLeftMotorController(leftMotorController);
  piLink->setRightMotorController(rightMotorController);

  pwmDriver->begin();
  sensorList->begin();
  controllerManager->begin();
  sensorList->loop();
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
    counter = 0;
    digitalWrite(LED_BUILTIN, LOW);
  }
  piLink->listen();
  ++counter;
}
