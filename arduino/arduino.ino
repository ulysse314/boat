#include "ArduinoController.h"
#include "BatteryController.h"
#include "ControllerManager.h"
#include "DallasSensor.h"
#include "GPSController.h"
#include "GPSSensor.h"
#include "InfoActuatorSensor.h"
#include "MemoryFree.h"
#include "MotorActuatorSensor.h"
#include "MotorController.h"
#include "OneWire.h"
#include "PiLink.h"
#include "PowerSensor.h"
#include "PWMDriver.h"
#include "SensorList.h"

#define ONE_WIRE_PIN          12
#define PWM_ADDRESS           0x40

SensorList *sensorList = NULL;
OneWire *oneWire = NULL;
InfoActuatorSensor *infoActuatorSensor = NULL;

ControllerManager *controllerManager = NULL;
ArduinoController *arduinoController = NULL;
BatteryController *batteryController = NULL;
GPSController *gpsController = NULL;
MotorController *leftMotorController = NULL;
MotorController *rightMotorController = NULL;
PWMDriver *pwmDriver = NULL;

PiLink *piLink = NULL;

void initGlobal() {
  sensorList = new SensorList();
  oneWire = new OneWire(ONE_WIRE_PIN);
  infoActuatorSensor = new InfoActuatorSensor();
  controllerManager = new ControllerManager();
  pwmDriver = new PWMDriver(&Wire, PWM_ADDRESS);
  piLink = PiLink::getPiLink();

  gpsController = new GPSController();
  controllerManager->addController(gpsController);
  arduinoController = new ArduinoController();
  controllerManager->addController(arduinoController);
  batteryController = new BatteryController(&Wire, oneWire);
  controllerManager->addController(batteryController);
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
  Wire.begin();
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
  piLink->listen();
  pwmDriver->loop();
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
  ++counter;
}
