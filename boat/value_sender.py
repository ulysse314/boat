import asyncio
import json
import logging
import queue

import ulysse_protocol

class ValueSender:
  sending_id = None
  values = {}
  ids_to_send = []
  logger = logging.getLogger("ValueSender")
  
  def __init__(self, delegate, boat_name, value_relay_server, boat_port, key):
    self.delegate = delegate
    self.boat_name = boat_name
    self.value_relay_server = value_relay_server
    self.boat_port = boat_port
    self.key = key
  
  def add_values(self, values):
    id = values["id"]
    self.values[id] = values
    self.ids_to_send.append(id)

  async def run(self):
    while True:
      try:
        self.logger.debug("Starting connection")
        loop = asyncio.get_event_loop()
        await loop.create_connection(lambda: ulysse_protocol.UlysseProtocol(self), self.value_relay_server, self.boat_port)
      except:
        self.logger.exception("connection failed")
        await asyncio.sleep(1)

  def send_next_values(self):
    if len(self.ids_to_send) == 0:
      return
    self.logger.debug("Sending next value")
    self.sending_id = self.ids_to_send[-1]
    self.transport.send_values(self.values[self.sending_id])

## connection
  def connection_made(self, transport):
    self.logger.info("Connection made")
    self.transport = transport
    self.transport.send_key(self.key)
    self.send_next_values()

  def connection_lost(self, ex):
    self.logger.info("Connection lost: {}".format(self.peername))

  def received_message(self, message):
    if "id" in message:
      self.ids_to_send.remove(message["id"])
      self.send_next_values()
      return
    self.delegate.received_values(values)

  def eof_received(self):
    self.transport.close()
    self.ids_to_send = None    
