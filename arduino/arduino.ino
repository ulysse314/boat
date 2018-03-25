#include <Adafruit_GPS.h>

#define GPSSerial Serial1
Adafruit_GPS GPS(&GPSSerial);

void setup() {
  while (!Serial);
  Serial.begin(115200);

  // 9600 baud is the default rate for the Ultimate GPS
  GPS.begin(115200);
  GPS.sendCommand(PMTK_SET_BAUD_115200);
  delay(1000);
  GPSSerial.end();
  delay(1000);
  GPS.begin(115200);
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_ALLDATA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
  GPS.sendCommand(PGCMD_ANTENNA);
}

uint32_t timer = millis();

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 10) {
    } else if (c == 13) {
      Serial.write(10);
      Serial.write(13);
    } else {
      GPSSerial.write(c);
    }
  }
  if (GPSSerial.available()) {
    char c = GPS.read();
    if (GPS.newNMEAreceived()) {
      char *nmea = GPS.lastNMEA();
      Serial.print(GPS.lastNMEA());
      GPS.parse(nmea);
    }
  }
  if (millis() - timer > 10000) {
    timer = millis(); // reset the timer
    Serial.print("\nTime: ");
    Serial.print(GPS.hour, DEC); Serial.print(':');
    Serial.print(GPS.minute, DEC); Serial.print(':');
    Serial.print(GPS.seconds, DEC); Serial.print('.');
    Serial.println(GPS.milliseconds);
    Serial.print("Date: ");
    Serial.print(GPS.day, DEC); Serial.print('/');
    Serial.print(GPS.month, DEC); Serial.print("/20");
    Serial.println(GPS.year, DEC);
    Serial.print("Fix: "); Serial.print((int)GPS.fix);
    Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
    Serial.print("Antenna: "); Serial.println((int)GPS.antenna);
    Serial.print("Sattelites in views: ");
    Serial.println((int)GPS.satellites_in_views);
    if (GPS.fix) {
      Serial.print("Location: ");
      Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
      Serial.print(", ");
      Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
      Serial.print("Speed (knots): "); Serial.println(GPS.speed);
      Serial.print("Angle: "); Serial.println(GPS.angle);
      Serial.print("Altitude: "); Serial.println(GPS.altitude);
      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
    }
  }
}
