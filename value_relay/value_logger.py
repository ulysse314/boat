#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import asyncio
import json
import logging
import pprint
import requests
import time

class ValueLogger:
  def __init__(self, boat_name, new_trip_url, logger_url):
    self.logger = logging.getLogger(self.__class__.__name__)
    self.logger.debug("Starting logger")
    self.new_trip_url = new_trip_url
    self.logger_url = logger_url
    self.trip_id = None
    self.should_stop = False
    self.boat_name = boat_name
  
  async def _send_packet(self, packet):
    self.logger.debug(self.logger_url + "&tripid=" + str(self.trip_id))
    resp = requests.post(self.logger_url + "&tripid=" + str(self.trip_id), data = {"values":packet})
    try_count = 0
    while True:
      if resp.status_code == 200:
        self.logger.debug("Packet sent")
        self.logger.debug(resp.text)
        break
      await asyncio.sleep(try_count)
      try_count += 1
      if try_count >= 10:
        self.logger.error("Fail to send, packet dropped")
        break
  
  async def _try_add_packet_soon(self, packet):
    try_count = 0
    while True:
      if self.loop != None:
        self.logger.debug("Adding packet to be sent")
        asyncio.run_coroutine_threadsafe(self._send_packet(packet), self.loop)
        break
      self.logger.debug("No event loop, try to send the packet later")
      await asyncio.sleep(try_count)
      try_count += 1
      if try_count >= 10:
        self.logger.error("Fail to get the event loop, packet dropped")
        break
  
  def add_packet(self, packet):
    if self.should_stop:
      self.logger.error("Should stop, packet dropped")
    else:
      asyncio.ensure_future(self._try_add_packet_soon(packet))

  def should_stop_when_empty(self):
    self.logger.debug("Should stop")
    self.call_soon_threadsafe(self.loop.stop)
    self.should_stop = True

  def run(self):
    self.logger.debug("start")
    self.loop = asyncio.new_event_loop()
    asyncio.set_event_loop(self.loop)
    while True:
      self.logger.debug(self.new_trip_url + "&boat=" + requests.utils.quote(self.boat_name))
      pprint.pprint(self.new_trip_url + "&boat=" + requests.utils.quote(self.boat_name))
      resp = requests.post(self.new_trip_url + "&boat=" + requests.utils.quote(self.boat_name))
      if resp.status_code == 200:
        data = json.loads(resp.text)
        self.logger.debug("Received " + resp.text)
        self.trip_id = data["trip_id"]
        self.logger.debug("Trip id: {}".format(self.trip_id))
        break
    self.loop.run_forever()
    pending = asyncio.Task.all_tasks()
    self.logger.debug("Wait until all stacks are done")
    self.loop.run_until_complete(asyncio.gather(*pending))
