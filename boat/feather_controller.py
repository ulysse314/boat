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
      self.dev_port = "/dev/featherm0"
    else:
      self.dev_port = dev_port
    self.port_speed = port_speed
    self.logger = logging.getLogger('FeatherController')
    self.logger.debug(pprint.pformat(self.dev_port))
    self.commands = []
    self.values = {}
    self.received_values = {}
    self.serial_transport = None

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
    self.values = { "gps": {} }
    try:
      if '0 GPS serial1' in self.received_values:
        values = self.received_values['0 GPS serial1']['values']
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

  def _parse_line(self, line):
    line = line.strip("\r\n \t")
    elements = line.split(" ")
    if len(elements) >= 4:
      try:
        key = "{} {} {}".format(elements[self.NODE_INDEX], elements[self.TYPE_INDEX], elements[self.ADDRESS_INDEX])
        self.received_values[key] = { "values": elements, "line": line, "update_time": time.time() }
      except:
        self.logger.exception("Problem to parse line")
    if line == "--":
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
