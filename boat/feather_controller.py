#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import asyncio
import json
import logging
import pprint
import os
import serial
import serial_asyncio
import sys
import time

parent_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
if parent_dir not in sys.path:
  sys.path.append(parent_dir)

import config
import line_protocol

class FeatherController:
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

  def set_motors(self, values):
    left = None
    right = None
    if "left%" in values:
      left = values["left%"]
    if "right%" in values:
      right = values["right%"]
    if self.serial_transport:
      self.next_motor_line = "Motor " + str(left) + " " + str(right) + "\n"
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
      except:
        self.logger.exception("connection failed")
      await asyncio.sleep(1)

  def _update_arduino_values(self, values):
    self.values['arduino'] = {}
    if values["stt"] != None: self.values["arduino"]["started"] = values["stt"]
    if values["cyc"] != None: self.values["arduino"]["cyclecount"] = values["cyc"]
    if values["ld"] != None: self.values["arduino"]["loopduration"] = values["ld"]
    if values["ct"] != None: self.values["arduino"]["computetime"] = values["ct"]
    if values["rf"] != None: self.values["arduino"]["ramfree"] = values["rf"]
    if values["rfd"] != None: self.values["arduino"]["ramfreediff"] = values["rfd"]
    if values["mil"] != None: self.values["arduino"]["millis"] = values["mil"]
    if values["tst"] != None: self.values["arduino"]["timestamp"] = values["tst"]
    if values["cmp"] != None: self.values["arduino"]["compiledate"] = values["cmp"]
    if values["ver"] != None: self.values["arduino"]["version"] = values["ver"]
    if "err" in values: self.values["arduino"]["errors"] = values["err"]

  def _update_battery_values(self, values):
    self.values['battery'] = {}
    if values["volt"] != None: self.values["battery"]["volt"] = values["volt"]
    if values["amp"] != None: self.values["battery"]["amp"] = values["amp"]
    if values["temp"] != None: self.values["battery"]["temp"] = values["temp"]
    if "err" in values: self.values["battery"]["errors"] = values["err"]

  def _update_gps_values(self, values):
    self.values['gps'] = {}
    if values["ant"] != None: self.values["gps"]["antenne"] = values["ant"]
    if values["vst"] != None: self.values["gps"]["sat"] = values["vst"]
    if values["ust"] != None: self.values["gps"]["tracked"] = values["ust"]
    if values["mod"] != None: self.values["gps"]["mode"] = values["mod"]
    if values["fxq"] != None: self.values["gps"]["fix"] = values["fxq"]
    if values["lat"] != None: self.values["gps"]["lat"] = values["lat"] / 10000000
    if values["lon"] != None: self.values["gps"]["lon"] = values["lon"] / 10000000
    if values["alt"] != None: self.values["gps"]["alt"] = values["alt"]
    if values["spd"] != None: self.values["gps"]["speed"] = values["spd"]
    if values["ang"] != None: self.values["gps"]["angle"] = values["ang"]
    if values["hdp"] != None: self.values["gps"]["dop.h"] = values["hdp"]
    if values["pdp"] != None: self.values["gps"]["dop.p"] = values["pdp"]
    if values["vdp"] != None: self.values["gps"]["dop.v"] = values["vdp"]
    if values["gh"] != None: self.values["gps"]["geoidheight"] = values["gh"]
    if "err" in values: self.values["gps"]["errors"] = values["err"]

  def _update_motor_values(self, values):
    motor_name = values["name"]
    self.values[motor_name] = {}
    if values["pwr"] != None: self.values[motor_name]["pwr"] = values["pwr"]
    if values["t"] != None: self.values[motor_name]["temp"] = values["t"]
    if "err" in values: self.values[motor_name]["errors"] = values["err"]

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
      if self.serial_transport:
        self.serial_transport.send_line("Motor ping")
 
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
    if values["name"] == "arduino":
      self._update_arduino_values(values)
    elif values["name"] == "bat":
      self._update_battery_values(values)
    elif values["name"] == "gps":
      self._update_gps_values(values)
    elif values["name"] == "lm":
      self._update_motor_values(values)
    elif values["name"] == "rm":
      self._update_motor_values(values)
    else:
      self.logger.warning("Unknown values " + pprint.pformat(values))

  def eof_received(self):
    self.serial_transport.close()

async def debug(arduino_controller):
  while True:
    await asyncio.sleep(1)
    pprint.pprint(arduino_controller.values)

def main(sensors, port):
  logging.basicConfig(level=logging.DEBUG)
  arduino_controller = FeatherController(sensors, port)
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
