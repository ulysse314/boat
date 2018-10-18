#include "Debug.h"

#include <Arduino.h>

void printDebugInfo(const char *fileName, unsigned int lineNumber, const char *info) {
  Serial.print(fileName);
  Serial.print(":");
  Serial.print(lineNumber);
  Serial.print(" ");
  Serial.println(info);
  Serial.flush();
}
