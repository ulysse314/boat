#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import asyncio
import json
import logging
import pprint
import os
import pprint
import serial
import serial_asyncio
import sys
import time

parent_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "../shared"))
if parent_dir not in sys.path:
  sys.path.append(parent_dir)

import boat_error
import config
import line_protocol

class ArduinoController:
  NODE_INDEX = 0
  TYPE_INDEX = 1
  ADDRESS_INDEX = 2
  SEND_MOTOR_LINE_DELAY = 0.1

  def __init__(self, sensors, dev_port = None, port_speed = 115200):
    self.sensors = sensors
    if not dev_port:
      self.dev_port = "/dev/featherm0"
    else:
      self.dev_port = dev_port
    self.port_speed = port_speed
    self.logger = logging.getLogger(self.__class__.__name__)
    self.logger.debug(pprint.pformat(self.dev_port))
    self.commands = []
    self.values = {}
    self.received_values = {}
    self.serial_transport = None
    self.next_motor_line = None
    self.last_send = 0
    self.send_pending = False
    self.last_line_received_timestamp = time.time()

  def send_ping(self):
    if self.serial_transport:
      self.serial_transport.send_line("ping")

  def set_motors(self, values):
    left = None
    right = None
    if "left%" in values:
      left = values["left%"]
    if "right%" in values:
      right = values["right%"]
    if self.serial_transport:
      self.next_motor_line = "lm " + str(left) + "\n" + "rm " + str(right) + "\n"
      asyncio.ensure_future(self._send_motor_line())

  async def _send_motor_line(self):
    current_time = time.time()
    if self.send_pending or self.next_motor_line is None:
      return
    if current_time - self.last_send < self.SEND_MOTOR_LINE_DELAY:
      self.send_pending = True
      await asyncio.sleep(self.SEND_MOTOR_LINE_DELAY - (current_time - self.last_send))
    self.send_pending = False
    self.serial_transport.send_line(self.next_motor_line)
    self.last_send = current_time
    self.next_motor_line = None

  def get_info(self):
    if self.serial_transport:
      self.serial_transport.send_line("Info")

  def set_leds(self, values):
    pass

  def start(self):
    asyncio.ensure_future(self._connect())
    asyncio.ensure_future(self._ping())

  def start_update_values(self):
    pass

  def stop_update_values(self):
    pass
  
  async def _connect(self):
    self.logger.debug("Connect")
    while True:
      try:
        self.logger.debug("Starting connection")
        if os.path.exists(self.dev_port):
          loop = asyncio.get_event_loop()
          await serial_asyncio.create_serial_connection(loop, lambda: line_protocol.LineProtocol(self), self.dev_port, baudrate = self.port_speed)
          break
        else:
          self.values = {}
          self.values['ard'] = { 'err': [[ boat_error.ArduinoDomain, boat_error.Arduino.DevNotFound ]] }
      except:
        self.values = {}
        self.values['ard'] = { 'err': [[ boat_error.ArduinoDomain, boat_error.Arduino.ConnectionError ]] }
        self.logger.exception("connection failed")
      await asyncio.sleep(1)

  def _update_gps_values(self, values):
    try:
      if "lat" in values and values["lat"] != None: values["lat"] = values["lat"] / 10000000
      if "lon" in values and values["lon"] != None: values["lon"] = values["lon"] / 10000000
    except:
      self.logger.exception(pprint.pformat(values))
    return values

  def _add_values(self, keys, value):
    array = self.values
    for key in keys[:-1]:
      if key not in array:
        array[key] = {}
      array = array[key]
    array[keys[-1]] = value

  async def _ping(self):
    while True:
      await asyncio.sleep(2)
      if time.time() - self.last_line_received_timestamp >= 2:
        self.values = {}
        self.values['ard'] = { 'err': [[ boat_error.ArduinoDomain, boat_error.Arduino.NoData ]] }
 
## connection
  def connection_made(self, transport):
    self.logger.info("Connection made")
    if self.serial_transport:
      self.serial_transport.delegate = None
    self.serial_transport = transport
    self.get_info()

  def connection_lost(self, ex):
    self.logger.info("Connection lost")
    peername = self.serial_transport.get_extra_info('peername')
    self.logger.info("Connection lost: {}".format(peername))
    self.serial_transport = None
    self.values = {}
    self.received_values = {}
    asyncio.ensure_future(self._connect())

  def received_message(self, line):
    line = line.strip("\r\n \t")
    try:
      values = json.loads(line)
    except:
      self.logger.exception("Cannot parse " + pprint.pformat(line))
      return
    name = values["name"]
    del values["name"]
    if name == "gps":
      values = self._update_gps_values(values)
    self.values[name] = values
    self.last_line_received_timestamp = time.time()

  def eof_received(self):
    self.serial_transport.close()

async def debug(arduino_controller):
  while True:
    await asyncio.sleep(1)
    pprint.pprint(arduino_controller.values)

def main(sensors, port):
  logging.basicConfig(level=logging.DEBUG)
  arduino_controller = ArduinoController(sensors, port)
  arduino_controller.start()
  task = asyncio.ensure_future(debug(arduino_controller))
  loop = asyncio.get_event_loop()
  loop.run_forever()
  return 0

if __name__ == "__main__":
  config.load()
  port = None
  if len(sys.argv) > 1:
    port = sys.argv[1]
  sys.exit(main(config.values["sensors"], port))
