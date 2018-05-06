#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import asyncio
import logging
import pprint
import os
import serial
import serial_asyncio
import sys
import time

import line_protocol

class ArduinoController:
  NODE_INDEX = 0
  TYPE_INDEX = 1
  ADDRESS_INDEX = 2

  def __init__(self, dev_port = None, port_speed = 115200):
    if not dev_port:
      self.dev_port = "/dev/ttyUSB0"
    else:
      self.dev_port = dev_port
    self.port_speed = port_speed
    self.logger = logging.getLogger(self.__class__.__name__)
    self.logger.debug(pprint.pformat(self.dev_port))
    self.commands = []
    self.values = {}
    self.received_values = {}
    self.serial_transport = None

  def set_motors(self, values):
    left = None
    right = None
    if "left%" in values:
      left = values["left%"]
    if "right%" in values:
      right = values["right%"]
    if self.serial_transport:
      line = str(left) + " " + str(right) + "\n"
      self.serial_transport.write(line)

  def set_leds(self, values):
    pass

  def start(self):
    asyncio.ensure_future(self._connect())

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

  def _update_values(self):
    self.values = { "motor": {} }
    try:
      if '1 DS18B20 28:0C:17:1F:03:00:00:08' in self.received_values:
        self.values["motor"]["lefttemp"] = float(self.received_values['1 DS18B20 28:0C:17:1F:03:00:00:08']['values'][3])
      else:
        self.values["motor"]["lefttemp"] = 0
      if '1 DS18B20 28:89:1A:1F:03:00:00:E5' in self.received_values:
        self.values["motor"]["righttemp"] = float(self.received_values['1 DS18B20 28:89:1A:1F:03:00:00:E5']['values'][3])
      else:
        self.values["motor"]["righttemp"] = 0
    except:
      self.logger.exception("Problem to get temps")
    try:
      self.values["dof"] = {
        "accel_x": float(self.received_values['1 LSM303 19']['values'][3]),
        "accel_y": float(self.received_values['1 LSM303 19']['values'][4]),
        "accel_z": float(self.received_values['1 LSM303 19']['values'][5]),
        "magnetic_x": float(self.received_values['1 LSM303 19']['values'][6]),
        "magnetic_y": float(self.received_values['1 LSM303 19']['values'][7]),
        "magnetic_z": float(self.received_values['1 LSM303 19']['values'][8]),
        "heading": float(self.received_values['1 LSM303 19']['values'][9]),
        "temp": float(self.received_values['1 BMP085 77']['values'][3]),
        "pressure": float(self.received_values['1 BMP085 77']['values'][4]) / 100.0,
      }
    except:
      self.values["dof"] = {
        "accel_x": 0,
        "accel_y": 0,
        "accel_z": 0,
        "magnetic_x": 0,
        "magnetic_y": 0,
        "magnetic_z": 0,
        "heading": 0,
        "temp": 0,
        "pressure": 0,
      }
      self.logger.exception("Problem to get DOF")
    try:
      self.values["battery"] = {
        "ampere": float(self.received_values[ '1 Ampere 14']['values'][3]) / 22.3,
        "ampere_raw": int(self.received_values[ '1 Ampere 14']['values'][3]),
        "volt": float(self.received_values[ '1 Volt 15']['values'][3]) / 73.0528200537,
        "volt_raw": int(self.received_values[ '1 Volt 15']['values'][3]),
      }
      if '1 DS18B20 28:6F:04:1F:03:00:00:A0' in self.received_values:
        self.values["battery"]["temp"] = float(self.received_values['1 DS18B20 28:6F:04:1F:03:00:00:A0']['values'][3])
      else:
        self.values["battery"]["temp"] = 0
    except:
      self.logger.exception("Problem to get battery info")
    try:
      self.values["water"] = {
        "detected": (int(self.received_values[ '1 Water 17']['values'][3]) < 700),
        "raw": int(self.received_values[ '1 Water 17']['values'][3])
      }
    except:
      self.logger.exception("Problem to get water info")

  def _parse_line(self, line):
    line = line.strip("\r\n \t")
    elements = line.split(" ")
    if len(elements) >= 4:
      try:
        key = "{} {} {}".format(elements[self.NODE_INDEX], elements[self.TYPE_INDEX], elements[self.ADDRESS_INDEX])
        self.received_values[key] = { "values": elements, "line": line, "update_time": time.time() }
      except:
        self.logger.exception("Problem to parse line")
    if line.startswith("-- "):
      result = True
    else:
      result = False
    return result
 
## connection
  def connection_made(self, transport):
    self.logger.info("Connection made")
    if self.serial_transport:
      self.serial_transport.delegate = None
    self.serial_transport = transport

  def connection_lost(self, ex):
    self.logger.info("Connection lost")
    peername = self.serial_transport.get_extra_info('peername')
    self.logger.info("Connection lost: {}".format(peername))
    self.serial_transport = None
    self.values = {}
    self.received_values = {}
    asyncio.ensure_future(self._connect())

  def received_message(self, line):
    if self._parse_line(line):
      self._update_values()

  def eof_received(self):
    self.serial_transport.close()

async def debug(arduino_controller):
  while True:
    await asyncio.sleep(1)
    pprint.pprint(arduino_controller.values)

def main(port):
  logging.basicConfig(level=logging.DEBUG)
  arduino_controller = ArduinoController(port)
  arduino_controller.start()
  task = asyncio.ensure_future(debug(arduino_controller))
  loop = asyncio.get_event_loop()
  loop.run_forever()
  return 0

if __name__ == "__main__":
  port = None
  if len(sys.argv) > 1:
    port = sys.argv[1]
  sys.exit(main(port))
