#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import asyncio
import json
import logging
import pprint
import queue
import sys

sys.path.append("..")

import config
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
    message_id = values["id"]
    self.logger.debug("Adding values with id {}".format(id))
    self.values[message_id] = values
    should_trigger = len(self.ids_to_send) == 0
    self.ids_to_send.append(message_id)
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
      message_id = message["id"]
      if message_id in self.ids_to_send:
        self.ids_to_send.remove(message_id)
        del self.values[message_id]
        self.send_next_values()
      return
    if self.delegate:
      self.delegate.received_values(values)

  def eof_received(self):
    self.transport.close()

async def debug_send_values(value_sender):
  idcount = 0
  while True:
    print("looop")
    value_sender.add_values({"id": idcount, "b": 1})
    idcount += 1
    await asyncio.sleep(10)

def main():
  pprint.pprint("test")
  logging.basicConfig(level=logging.DEBUG)
  boat_name = sys.argv[1]
  config.load(boat_name)
  value_sender = ValueSender(boat_name, config.values["value_relay_server"], int(config.values["boat_port"]), config.values["boat_key"])
  value_sender.start()
  asyncio.ensure_future(debug_send_values(value_sender))
  print("ok")
  asyncio.get_event_loop().run_forever()

if __name__ == "__main__":
  main()
