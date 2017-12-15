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
last_packet = None
logger = None

def send_packet_to_all_valid_controls(packet):
  global last_packet
  global valid_controller_clients
  last_packet = packet
  for control in valid_controller_clients:
    control.send_packet(packet)

class GenericClient(asyncio.Protocol):
  partial_packet = b''
  received_hello_packet = False
  data_to_send = b''
  pending_packets_to_send = []
  paused = False

  def __init__(self):
    self.logger = logging.getLogger(self.__class__.__name__)

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
    eol_index = data.find(b'\n')
    if eol_index == -1:
      self.partial_packet += data
      return
    end_of_buffer_index = eol_index
    if eol_index > 0 and data[eol_index] == b'\r':
      end_of_buffer_index -= 1
    full_packet = self.partial_packet + data[:end_of_buffer_index - 1]
    if len(full_packet) == 0:
      return
    self.partial_packet = data[eol_index + 1:]
    if not self.received_hello_packet:
      self.test_hello_packet(full_packet.decode("utf-8"))
      return
    try:
      message = json.loads(full_packet.decode("utf-8"))
      if "id" in message:
        self.send_packet(('{"id":' + str(message["id"]) + '}').encode("utf-8"))
      self.message_received(message, full_packet)
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

class BoatClient(GenericClient):
  key_name = "boat_key"

  def hello_packet_received(self):
    self.logger.debug("New boat")
    global valid_boat_client
    super(BoatClient, self).hello_packet_received()
    if valid_boat_client:
      valid_boat_client.transport.close()
    valid_boat_client = self

  def message_received(self, message, packet):
    global logger
    send_packet_to_all_valid_controls(packet)
    if logger:
      logger.add_packet(packet)

  def connection_lost(self, ex):
    global valid_boat_client
    if self == valid_boat_client:
      valid_boat_client = None
    super(BoatClient, self).connection_lost(ex)

class ControllerClient(GenericClient):
  key_name = "controller_key"

  def hello_packet_received(self):
    self.logger.debug("New controller")
    global valid_controller_clients
    global last_packet
    super(ControllerClient, self).hello_packet_received()
    valid_controller_clients.append(self)
    if last_packet:
      self.send_packet(last_packet)

  def message_received(self, message, packet):
    if valid_boat_client:
      valid_boat_client.send_packet(packet)
    if "record" in message:
      global logger
      if message["record"] and not logger:
        global BOAT_NAME
        global config
        logger = value_logger.ValueLogger(BOAT_NAME, config.values["new_trip_url"], config.values["logger_url"])
        logger.start()
      elif not message["record"] and logger:
        logger.should_stop()
        logger = None
    if "camera" in message and "state" in message["camera"]:
      if message["camera"]["state"]:
        camera.start()
      else:
        camera.stop()

  def connection_lost(self, ex):
    global valid_controller_clients
    if self.received_hello_packet:
      valid_controller_clients.remove(self)
    super(ControllerClient, self).connection_lost(ex)

if __name__ == '__main__':
    print("starting up..")

    loop = asyncio.get_event_loop()
    logging.debug("Boat server listenning for port: {}".format(BOAT_PORT))
    boat_coro = loop.create_server(lambda: BoatClient(), port = BOAT_PORT)
    boat_server = loop.run_until_complete(boat_coro)
    logging.debug("Controller server listenning for port: {}".format(CONTROLLER_PORT))
    controller_coro = loop.create_server(lambda: ControllerClient(), port = CONTROLLER_PORT)
    controller_server = loop.run_until_complete(controller_coro)
    loop.run_forever()
