#!/usr/bin/env python3
# -*- coding: utf-8 -*-

parent_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "../shared"))
if parent_dir not in sys.path:
  sys.path.append(parent_dir)

ArduinoDomain = 1
E3372Domain = 5
PiDomain = 6

class Arduino:
  NoError = 0
  DevNotFound = 100
  ConnectionError = 101
  NoData = 102

class E3372:
  NoError = 0
  GenericError = 1
  NoInterface = 2
  SignalURLFailed = 3
  StatusURLFailed = 4
  TrafficStatURLFailed = 5
  NotConnected = 6
  LowSignal = 7
  VeryLowSignal = 8
  SimLocked = 9

class Pi:
  noError = 0
  temperatureInfo = 1
  temperatureWarning = 2
  temperatureCritical = 3
  memoryInfo = 4
  memoryWarning = 5
  memoryCritical = 6
  diskInfo = 7
  diskWarning = 8
  diskCritical = 9
  cpuInfo = 10
  cpuWarning = 11
  cpuCritical = 12
