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
    self.values = { "gps": {}, "motor": {} }
    for sensor_id in self.received_values:
      values = self.received_values[sensor_id]['values']
      if sensor_id == '0 GPS serial1':
        try:
          if len(values) >= 4: self.values["gps"]["antenne"] = int(values[3]);
          if len(values) >= 5: self.values["gps"]["sat"] = int(values[4])
          if len(values) >= 6: self.values["gps"]["tracked"] = int(values[5])
          if len(values) >= 7: self.values["gps"]["mode"] = int(values[6])
          if len(values) >= 8: self.values["gps"]["fix"] = int(values[7])
          if len(values) >= 9: self.values["gps"]["lat"] = float(values[8]) / 10000000
          if len(values) >= 10: self.values["gps"]["lon"] = float(values[9]) / 10000000
          if len(values) >= 11: self.values["gps"]["alt"] = float(values[10])
          if len(values) >= 12: self.values["gps"]["speed"] = float(values[11])
          if len(values) >= 13: self.values["gps"]["angle"] = float(values[12])
        except:
          self.logger.exception("Problem to get gps")
      elif sensor_id == "0 Motor Motor":
        self.values["motor"]["left%"] = values[3]
        self.values["motor"]["right%"] = values[4]
        self.values["motor"]["leftresult"] = values[5]
        self.values["motor"]["rightresult%"] = values[6]
      elif sensor_id in self.sensors:
        sensor = self.sensors[sensor_id]
        if sensor["type"] == "dallas":
          self._add_values(sensor["keys"], float(values[3]))
      else:
        self.logger.debug("Unknown line: " + self.received_values[sensor_id]['line'])

  def _add_values(self, keys, value):
    array = self.values
    for key in keys[:-1]:
      if key not in array:
        array[key] = {}
      array = array[key]
    array[keys[-1]] = value

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
