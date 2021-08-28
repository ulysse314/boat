#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# test_mavlink_client.py

import pprint
import socket
import sys

sys.path.append("mavlink")
from MavlinkGenerated import mavlink

class Toto:
  def __init__(self, client):
    self.client = client

  def write(self, buff):
    n = client.send(buff)

server_address = ('127.0.0.1', 5760 )

print("Connect to {}".format(server_address))
client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.connect(server_address)

def receive_message(message):
  print("====== receive")
#  pprint.pprint(vars(message))
#  pprint.pprint(dir(message))
  pprint.pprint(message.get_type())
  pprint.pprint(message.id)
  pprint.pprint(mavlink.MAVLINK_MSG_ID_HEARTBEAT)
  pprint.pprint(message.type)


test = mavlink.MAVLink(Toto(client))
test.set_callback(receive_message)
test.heartbeat_send(mavlink.MAV_TYPE_SURFACE_BOAT, mavlink.MAV_AUTOPILOT_GENERIC, mavlink.MAV_MODE_FLAG_SAFETY_ARMED, 0, mavlink.MAV_STATE_BOOT)
test.heartbeat_send(mavlink.MAV_TYPE_SUBMARINE, mavlink.MAV_AUTOPILOT_GENERIC, mavlink.MAV_MODE_FLAG_SAFETY_ARMED, 0, mavlink.MAV_STATE_BOOT)

print('Reception...')
donnees = client.recv(1024)
print('Recu : {} {}'.format(len(donnees), donnees))
if len(donnees) < 30:
  donnees += client.recv(1024)
print('Recu : {} {}'.format(len(donnees), donnees))
test.parse_char(donnees)
while test.parse_char("".encode()) is not None:
  pass

print('Deconnexion.')
client.close()
