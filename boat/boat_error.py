#!/usr/bin/env python3
# -*- coding: utf-8 -*-

ArduinoDomain = 1
E3372Domain = 5

class Arduino:
  NoError = 0

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
