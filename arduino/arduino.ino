#include "ADS1115Sensor.h"
#include "ArduinoController.h"
#include "BatteryController.h"
#include "ControllerManager.h"
#include "DallasSensor.h"
#include "DriverManager.h"
#include "GPSController.h"
#include "GPSSensor.h"
#include "HardwareConfig.h"
#include "HullController.h"
#include "MemoryFree.h"
#include "MotorController.h"
#include "PiLink.h"
#include "PowerSensor.h"
#include "PWMDriver.h"
#include "SensorList.h"

#include <OneWire.h>

HardwareConfig hardwareConfig;

SensorList *sensorList = NULL;
ADS1115Sensor *ads1115Sensor = NULL;

DriverManager *driverManager = NULL;
PWMDriver *pwmDriver = NULL;

ControllerManager *controllerManager = NULL;
ArduinoController *arduinoController = NULL;
BatteryController *batteryController = NULL;
HullController *hullController = NULL;
GPSController *gpsController = NULL;
MotorController *leftMotorController = NULL;
MotorController *rightMotorController = NULL;

PiLink *piLink = NULL;

void initGlobal() {
  controllerManager = new ControllerManager();
  driverManager = new DriverManager();
  piLink = PiLink::generatePiLink(&hardwareConfig);

  sensorList = new SensorList();
  ads1115Sensor = new ADS1115Sensor(hardwareConfig.ads1115Address(), hardwareConfig.i2c());
  sensorList->addSensor(ads1115Sensor);

  pwmDriver = new PWMDriver(&hardwareConfig);
  driverManager->addDriver(pwmDriver);

  gpsController = new GPSController(&hardwareConfig);
  controllerManager->addController(gpsController);
  arduinoController = ArduinoController::generateController(&hardwareConfig);
  controllerManager->addController(arduinoController);
  batteryController = new BatteryController(ads1115Sensor, &hardwareConfig);
  controllerManager->addController(batteryController);
  hullController = new HullController(ads1115Sensor, &hardwareConfig);
  controllerManager->addController(hullController);
  leftMotorController = MotorController::LeftMotor(pwmDriver, &hardwareConfig);
  controllerManager->addController(leftMotorController);
  rightMotorController = MotorController::RightMotor(pwmDriver, &hardwareConfig);
  controllerManager->addController(rightMotorController);

  controllerManager->addSensorsToList(sensorList);
  piLink->setLeftMotorController(leftMotorController);
  piLink->setRightMotorController(rightMotorController);
  piLink->setArduinoController(arduinoController);

  driverManager->begin();
  sensorList->begin();
  controllerManager->begin();
  sensorList->loop();
  leftMotorController->setValue(0);
  rightMotorController->setValue(0);
}


void setup() {
  hardwareConfig.begin();
  for (int i = 0; i < 4; i++) {
    digitalWrite(hardwareConfig.ledPin(), HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(500);                       // wait for a second
    digitalWrite(hardwareConfig.ledPin(), LOW);    // turn the LED off by making the voltage LOW
    delay(500);                       // wait for a second
    Serial.println(i);
  }
  Serial.println("Starting...");
  initGlobal();
  Serial.println("Started!");
}

void loop() {
  static unsigned long loopCounter = 0;
  static unsigned long lastLoopTime = millis();
  static unsigned long longestLoopDuration = 0;
  static unsigned long lastCycleTime = 1000 * (unsigned long)(millis() / 1000.);
  sensorList->loop();
  piLink->listen();
  driverManager->loop();
  unsigned long currentTime = millis();
  unsigned long loopDuration = currentTime - lastLoopTime;
  if (loopDuration > longestLoopDuration) {
    longestLoopDuration = loopDuration;
  }
  lastLoopTime = currentTime;
  unsigned long cycleDuration = currentTime - lastCycleTime;
  if ((unsigned long)(currentTime - lastCycleTime) >= 1000) {
    digitalWrite(hardwareConfig.ledPin(), HIGH);
    lastCycleTime = 1000 * (unsigned long)(currentTime / 1000.) ;

    arduinoController->setLoopCounter(loopCounter);
    arduinoController->setCycleDuration(cycleDuration);
    arduinoController->setLongestLoopDuration(longestLoopDuration);
    controllerManager->sensorsHasBeenUpdated();
    controllerManager->outputControllers(piLink);
    sensorList->readValues();
    arduinoController->setExportDuration(millis() - currentTime);
    loopCounter = 0;
    longestLoopDuration = 0;
    digitalWrite(hardwareConfig.ledPin(), LOW);
  }
  ++loopCounter;
}
