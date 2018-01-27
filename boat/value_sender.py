#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import asyncio
import json
import logging
import pprint
import queue

import json_protocol

class ValueSender:
  sending_id = None
  values = {}
  ids_to_send = []
  logger = logging.getLogger("ValueSender")
  transport = None
  
  def __init__(self, boat_name, value_relay_server, boat_port, key):
    self.boat_name = boat_name
    self.value_relay_server = value_relay_server
    self.boat_port = boat_port
    self.key = key
  
  def start(self):
    self.logger.debug("Start")
    asyncio.ensure_future(self.connect())

  def add_values(self, values):
    id = values["id"]
    self.logger.debug("Adding values with id {}".format(id))
    self.values[id] = values
    should_trigger = len(self.ids_to_send) == 0
    self.ids_to_send.append(id)
    if should_trigger:
      self.send_next_values()

  async def connect(self):
    while True:
      try:
        self.logger.debug("Starting connection {} {}".format(self.value_relay_server, self.boat_port))
        loop = asyncio.get_event_loop()
        await loop.create_connection(lambda: json_protocol.JsonProtocol(self), self.value_relay_server, self.boat_port)
        self.logger.debug("Starting done")
        break
      except:
        self.logger.error("Connection failed")
        await asyncio.sleep(1)

  def send_next_values(self):
    if len(self.ids_to_send) == 0 or not self.transport:
      return
    self.sending_id = self.ids_to_send[-1]
    self.logger.debug("Sending next value {}".format(self.sending_id))
    self.transport.send_json(self.values[self.sending_id])

## connection
  def connection_made(self, transport):
    self.logger.info("Connection made: {}".format(pprint.pformat(transport)))
    if self.transport:
      self.transport.delegate = None
    self.transport = transport
    self.transport.send_line(self.key)
    self.send_next_values()

  def connection_lost(self, ex):
    peername = self.transport.get_extra_info('peername')
    self.logger.info("Connection lost: {}".format(peername))
    self.transport = None
    asyncio.ensure_future(self.connect())

  def received_message(self, message):
    if "id" in message:
      self.ids_to_send.remove(message["id"])
      self.send_next_values()
      return
    if self.delegate:
      self.delegate.received_values(values)

  def eof_received(self):
    self.transport.close()
