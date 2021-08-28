#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# test_mavlink_server.py

import pprint
import socket
import sys

sys.path.append("mavlink")
from MavlinkGenerated import mavlink

def receive_message(message):
  print("====== receive")
#  pprint.pprint(vars(message))
#  pprint.pprint(dir(message))
  pprint.pprint(message.get_type())
  pprint.pprint(message.id)
  pprint.pprint(mavlink.MAVLINK_MSG_ID_HEARTBEAT)
  pprint.pprint(message.type)

class Toto:
  def write(self, buff):
    pprint.pprint(buff)

def udp(server_address, link):
  print("Start UDP")
  sock = socket.socket(socket.AF_INET, # Internet
                       socket.SOCK_DGRAM) # UDP
  sock.bind(server_address)
  while True:
    data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
    print("received message: %s" % data)

def sendudp(server_address, link):
  print("Send UDP")
  sock = socket.socket(socket.AF_INET, # Internet
                       socket.SOCK_DGRAM) # UDP
  sock.sendto(b'TOTO', server_address)

def tcp(server_address, link):
  print("Start TCP")
  sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
  sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

  print('starting up on {} port {}'.format(server_address[0], server_address[1]))
  sock.bind(server_address)
  # Listen for incoming connections
  sock.listen(1)
  while True:
    # Wait for a connection
    print('waiting for a connection')
    connection, client_address = sock.accept()
    try:
      print('connection from {}'.format(client_address))
      # Receive the data in small chunks and retransmit it
      while True:
        try:
          data = connection.recv(1024)
        except:
          break
        print('received {}, {}'.format(len(data), data))
        if data:
          print('sending data back to the client')
          link.parse_char(data)
          while link.parse_char("".encode()) is not None:
            pass
          print("parse done")
        else:
          print('no more data from {}'.format(client_address))
          break
    finally:
      # Clean up the connection
      connection.close()

link = mavlink.MAVLink(Toto())
link.heartbeat_send(mavlink.MAV_TYPE_SURFACE_BOAT, mavlink.MAV_AUTOPILOT_GENERIC, mavlink.MAV_MODE_FLAG_SAFETY_ARMED, 0, mavlink.MAV_STATE_BOOT)
link.set_callback(receive_message)
server_address = ('', 5760)
if "tcp" in sys.argv:
  tcp(server_address, link)
elif "udp" in sys.argv:
  udp(server_address, link)
elif "sendudp" in sys.argv:
  sendudp(server_address, link)
