#!/usr/bin/env python3

import asyncio
import json
import logging
import math
import os
import pprint
import time

import json_protocol

class GPSController():
  SERVER = "127.0.0.1"
  PORT = 2947
  WATCH = '?WATCH={"enable":true,"json":true};'
  logger = logging.getLogger("ValueSender")
  json_transport = None
  response_available_event = asyncio.Event()
  received_values = {}
  values = {}
  
  def start(self):
    asyncio.ensure_future(self.run())
    asyncio.ensure_future(self.connect())

  async def connect(self):
    while True:
      try:
        self.logger.debug("Starting connection")
        loop = asyncio.get_event_loop()
        await loop.create_connection(lambda: json_protocol.JsonProtocol(self), self.SERVER, self.PORT)
        self.logger.debug("Starting done")
        break
      except:
        self.logger.exception("connection failed")
        await asyncio.sleep(1)

  async def run(self):
    while True:
      values = self._get_values()
      self.values = { "gps": values }
      await asyncio.sleep(1)

  def _get_values(self):
    return self.received_values.copy()
 
## connection
  def connection_made(self, transport):
    self.logger.info("Connection made")
    if self.json_transport:
      self.json_transport.delegate = None
    self.json_transport = transport
    self.json_transport.send_data(self.WATCH)

  def connection_lost(self, ex):
    peername = self.json_transport.get_extra_info('peername')
    self.logger.info("Connection lost: {}".format(peername))
    self.json_transport = None
    self.received_values = {}
    asyncio.ensure_future(self.connect())

  def received_message(self, message):
    if "class" not in message:
      self.logger.warning("No class value in {}".format(pprint.pformat(message)))
    elif message["class"] == "TPV":
      for key in ["alt", "climb", "epc", "epd", "eps", "ept", "epv", "epx", "epy", "lat", "lon", "speed", "track", "mode"]:
        if key in message:
          self.received_values[key] = message[key]
        elif key in self.received_values:
          del self.received_values[key]
    elif message["class"] == "SKY":
      if "satellites" in message:
        if "sat" in self.received_values:
          del self.received_values["sat"]
        if "tracked" in self.received_values:
          del self.received_values["tracked"]
        satellites = message["satellites"]
        self.received_values["sat"] = len(satellites)
        tracked = 0
        for sat in satellites:
          if "used" in sat and sat["used"]:
            tracked += 1
        self.received_values["tracked"] = tracked

  def eof_received(self):
    self.json_transport.close()

async def debug(gps_controller):
  while True:
    pprint.pprint(gps_controller.values)
    await asyncio.sleep(1)

def main():
  gps_controller = GPSController()
  gps_controller.start()
  task = asyncio.ensure_future(debug(gps_controller))
  loop = asyncio.get_event_loop()
  loop.run_forever()

if __name__ == "__main__":
  main()