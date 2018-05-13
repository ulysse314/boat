#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import subprocess
import time

class CommandController:
  def __init__(self, arduino_controller):
    self.values = {}
    self.boot_timestamp = time.time()
    self.arduino_controller = arduino_controller

## controller methods
  def start(self):
    pass

  def start_update_values(self):
    pass

  def stop_update_values(self):
    self.values = {}

## commands
  def execute(self, command):
    if command == "reboot":
      self.reboot()
    elif command == "shutdown":
      self.shutdown()
    elif command == "arduino_update":
      self.arduino_update()
    elif command == "arduino_info":
      self.get_arduino_info()

  def reboot(self):
    subprocess.call(["/sbin/reboot"])

  def shutdown(self):
    subprocess.call(["/sbin/shutdown", "-h", "now"])

  def arduino_update(self):
    subprocess.call(["/home/ulysse314/scripts/arduino/update.sh"])

  def get_arduino_info(self):
    self.arduino_controller.get_info()
