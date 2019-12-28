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
#include "InfoActuatorSensor.h"
#include "MemoryFree.h"
#include "MotorController.h"
#include "PiLink.h"
#include "PowerSensor.h"
#include "PWMDriver.h"
#include "SensorList.h"

#include <OneWire.h>

HardwareConfig hardwareConfig;

SensorList *sensorList = NULL;
InfoActuatorSensor *infoActuatorSensor = NULL;
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
  infoActuatorSensor = new InfoActuatorSensor();
  controllerManager = new ControllerManager();
  driverManager = new DriverManager();
  piLink = PiLink::getPiLink();

  sensorList = new SensorList();
  ads1115Sensor = new ADS1115Sensor(hardwareConfig.getADS1115Address(), hardwareConfig.getI2C());
  sensorList->addSensor(ads1115Sensor);

  pwmDriver = new PWMDriver(&hardwareConfig);
  driverManager->addDriver(pwmDriver);

  gpsController = new GPSController();
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

  hardwareConfig.begin();
  driverManager->begin();
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
  driverManager->loop();
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
