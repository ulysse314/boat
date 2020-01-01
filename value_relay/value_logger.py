#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import aiohttp
import asyncio
import json
import logging
import pprint
import requests
import time

async def fetch(url, data = None):
  async with aiohttp.ClientSession() as client:
    async with client.post(url, data = data) as resp:
      status = resp.status
      html = await resp.text()
      return (status, html)

class ValueLogger:
  def __init__(self, boat_name, new_trip_url, logger_url):
    self.logger = logging.getLogger(self.__class__.__name__)
    self.logger.debug("Starting logger")
    self.new_trip_url = new_trip_url
    self.logger_url = logger_url
    self.trip_id = None
    self.should_stop = False
    self.boat_name = boat_name
    self.packets_to_send = None
  
  async def _send_packet(self, packet):
    url = self.logger_url + "&tripid=" + str(self.trip_id)
    try_count = 0
    while True:
      try:
        start_time = time.time()
        (status, html) = await fetch(url, {"values": packet.decode("utf-8")})
        upload_duration = time.time() - start_time
        self.logger.debug("Upload duration: {}, size: {}".format(upload_duration, len(packet)))
        if status == 200:
          self.logger.debug("Packet sent {}".format(html))
          break
      except:
        self.logger.exception("Fail to send packet")
      await asyncio.sleep(try_count)
      try_count += 1
      if try_count >= 10:
        self.logger.error("Fail to send, packet dropped")
        break
  
  async def _try_add_packet_soon(self):
    while len(self.packets_to_send) > 0:
      packet = self.packets_to_send.pop(0)
      await self._send_packet(packet)
    self.packets_to_send = None

  def _start_sending_if_needed(self):
    if self.trip_id != None and self.packets_to_send != None:
      asyncio.ensure_future(self._try_add_packet_soon())

  async def _run(self):
    self.logger.debug("run")
    try_count = 0
    while True:
      url = self.new_trip_url + "&boat=" + requests.utils.quote(self.boat_name)
      self.logger.debug("URL: {}".format(url))
      (status, html) = await fetch(url)
      if status == 200:
        try:
          data = json.loads(html)
          self.logger.debug("Received " + html)
          self.trip_id = data["trip_id"]
          self.logger.debug("Trip id: {}".format(self.trip_id))
          break
        except:
          self.logger.exception("Fail to get the trip id")
      try_count += 1
      if try_count >= 10:
        self.logger.error("Fail to get the trip id, stopping")
        self.should_stop = True
    self._start_sending_if_needed()
  
  def add_packet(self, packet):
    if self.should_stop:
      self.logger.error("Should stop, packet dropped")
    else:
      if self.packets_to_send != None:
        self.packets_to_send.append(packet)
      else:
        self.packets_to_send = [packet]
      self._start_sending_if_needed()

  def start(self):
    self.logger.debug("start")
    asyncio.ensure_future(self._run())

  def stop(self):
    self.logger.debug("Should stop")
    self.should_stop = True

async def debug(value_logger):
  await asyncio.sleep(2)
  value_logger.add_packet('{"test":2,"timestamp":0}')

if __name__ == '__main__':
  print("starting up..")
  logging.basicConfig(level=logging.DEBUG)
  loop = asyncio.get_event_loop()
  value_logger = ValueLogger("test", generate_trip, save_value)
  value_logger.start()
  value_logger.add_packet('{"test":1,"timestamp":0}')
  asyncio.ensure_future(debug(value_logger))
  loop.run_forever()
