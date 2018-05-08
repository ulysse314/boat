#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Value relay

import asyncio
import json
import logging
import os
import pprint
import sys

os.chdir(os.path.abspath(os.path.dirname(sys.argv[0])))
sys.path.append("..")

import camera
import config
import line_parser
import value_logger

if len(sys.argv) == 1:
  with open("/etc/ulysse314/name", "r") as file:
    BOAT_NAME = file.readline().strip()
else:
  BOAT_NAME = sys.argv[1]

logging.basicConfig(level=logging.DEBUG)
logging.debug("Boat: {}".format(BOAT_NAME))
config.load(BOAT_NAME)

BOAT_PORT = int(config.values["boat_port"])
CONTROLLER_PORT = int(config.values["controller_port"])

valid_boat_client = None
valid_controller_clients = []
last_message = None
last_message_id_for_boat = None
logger_for_values = None
extra_values = { "record": False, "camera": { "state": False } }

def send_message_to_all_valid_controls(message):
  global last_message
  global last_message_id_for_boat
  global valid_controller_clients
  if last_message_id_for_boat == None or last_message_id_for_boat < message["id"]:
    last_message_id_for_boat = message["id"]
    last_message = message
    for control in valid_controller_clients:
      control.send_message(message)

class GenericClient(asyncio.Protocol):
  def __init__(self):
    self.line_parser = line_parser.LineParser()
    self.logger = logging.getLogger(self.__class__.__name__)
    self.received_hello_packet = False
    self.data_to_send = b''
    self.pending_packets_to_send = []
    self.paused = False

  def connection_made(self, transport):
    self.transport = transport
    self.peername = transport.get_extra_info("peername")
    self.logger.debug(self.peername)
    
  def test_hello_packet(self, hello_packet):
    key = config.values[self.key_name]
    if key != hello_packet:
      pprint.pprint(key)
      pprint.pprint(hello_packet)
      self.logger.error("key not valid")
      self.transport.close()
      return
    self.logger.debug("key valid")
    self.hello_packet_received()
    return

  def hello_packet_received(self):
    self.received_hello_packet = True

  def data_received(self, data):
    list = self.line_parser.add_buffer(data)
    for line in list:
      self.line_ready(line)

  def line_ready(self, line):
    if not self.received_hello_packet:
      self.test_hello_packet(line.decode("utf-8"))
      return
    try:
      message = json.loads(line.decode("utf-8"))
      if "id" in message:
        self.send_packet(('{"id":' + str(message["id"]) + '}').encode("utf-8"))
      self.message_received(message, line)
    except:
      self.logger.exception("read value")
      pprint.pprint(data)
      self.transport.close()

  def send_packet(self, packet):
    if self.paused:
      self.queue_packet(packet)
    else:
      self.logger.debug("Write message to {}".format(self.peername))
      self.transport.write(packet + b'\n')

  def queue_packet(self, packet):
    self.pending_packets_to_send.append(packet)

  def connection_lost(self, ex):
    self.logger.debug("connection_lost: {}".format(self.peername))
    
  def pause_writing(self):
    self.logger.debug("Paused: {}".format(self.peername))
    self.paused = True
    
  def resume_writing(self):
    self.logger.debug("Resumed: {}".format(self.peername))
    self.paused = False

  def send_message(self, message):
    line = json.dumps(message)
    self.send_packet(line.encode("utf-8"))

class BoatClient(GenericClient):
  def __init__(self):
    self.key_name = "boat_key"
    super(BoatClient, self).__init__()

  def hello_packet_received(self):
    global last_message_id_for_boat
    self.logger.debug("New boat")
    global valid_boat_client
    super(BoatClient, self).hello_packet_received()
    if valid_boat_client:
      valid_boat_client.transport.close()
    last_message_id_for_boat = None
    valid_boat_client = self

  def message_received(self, message, packet):
    global logger_for_values
    send_message_to_all_valid_controls(message)
    if logger_for_values:
      logger_for_values.add_packet(packet)

  def connection_lost(self, ex):
    global valid_boat_client
    if self == valid_boat_client:
      valid_boat_client = None
    super(BoatClient, self).connection_lost(ex)

class ControllerClient(GenericClient):
  def __init__(self, boat_name):
    self.key_name = "controller_key"
    self.boat_name = boat_name
    super(ControllerClient, self).__init__()

  def hello_packet_received(self):
    self.logger.debug("New controller")
    global valid_controller_clients
    global last_message
    super(ControllerClient, self).hello_packet_received()
    valid_controller_clients.append(self)
    if valid_boat_client:
      valid_boat_client.send_message({"controller":len(valid_controller_clients)})
    if last_message:
      self.send_message(last_message)

  def message_received(self, message, packet):
    global logger_for_values
    global extra_values
    if valid_boat_client:
      valid_boat_client.send_packet(packet)
    if "record" in message:
      if message["record"] and not logger_for_values:
        global config
        logger_for_values = value_logger.ValueLogger(self.boat_name, config.values["new_trip_url"], config.values["logger_url"])
        logger_for_values.start()
        extra_values["record"] = True
      elif not message["record"] and logger_for_values:
        logger_for_values.stop()
        logger_for_values = None
        extra_values["record"] = False
    if "camera" in message:
      if "state" in message["camera"]:
        if message["camera"]["state"]:
          camera.start()
        else:
          camera.stop()
      extra_values["camera"].update(message["camera"])

  def send_message(self, message):
    global extra_values
    message = message.copy()
    message.update(extra_values)
    super(ControllerClient, self).send_message(message)

  def connection_lost(self, ex):
    global valid_controller_clients
    global valid_boat_client
    if self.received_hello_packet:
      valid_controller_clients.remove(self)
      if valid_boat_client:
        valid_boat_client.send_message({"controller":len(valid_controller_clients)})
    super(ControllerClient, self).connection_lost(ex)

if __name__ == '__main__':
  print("starting up..")
  loop = asyncio.get_event_loop()
  logging.debug("Boat server listenning for port: {}".format(BOAT_PORT))
  boat_coro = loop.create_server(lambda: BoatClient(), port = BOAT_PORT)
  boat_server = loop.run_until_complete(boat_coro)
  logging.debug("Controller server listenning for port: {}".format(CONTROLLER_PORT))
  controller_coro = loop.create_server(lambda: ControllerClient(BOAT_NAME), port = CONTROLLER_PORT)
  controller_server = loop.run_until_complete(controller_coro)
  loop.run_forever()
