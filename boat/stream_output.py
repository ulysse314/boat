#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Boat

import asyncio
import io
import json
import logging
import os
import pprint
import sys
from threading import Condition

parent_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
if parent_dir not in sys.path:
  sys.path.append(parent_dir)

import arduino_controller
import boat_controller
import command_controller
import config
import e3372_controller
import gps_controller
import munin_server
import pi_controller
import pwm_controller
import value_sender

if len(sys.argv) == 1:
  BOAT_NAME = config.default_boat_name()
else:
  BOAT_NAME = sys.argv[1]

logging.basicConfig(level=logging.DEBUG)
config.load(BOAT_NAME)
import socket
import time
import picamera

class StreamingOutput(object):
  def __init__(self):
    self.frame = None
    self.buffer = io.BytesIO()
    self.condition = Condition()

  def write(self, buf):
    if buf.startswith(b'\xff\xd8'):
      # New frame, copy the existing buffer's content and notify all
      # clients it's available
      self.buffer.truncate()
      with self.condition:
        self.frame = self.buffer.getvalue()
        self.condition.notify_all()
      self.buffer.seek(0)
    return self.buffer.write(buf)

# Connect a client socket to my_server:8000 (change my_server to the
# hostname of your server)
pprint.pprint((config.values["value_relay_server"], int(config.values["boat_stream_port"])))
try:
  with picamera.PiCamera() as camera:
    camera.resolution = (640, 480)
    camera.framerate = 24
    # Start a preview and let the camera warm up for 2 seconds
    camera.start_preview()
    time.sleep(2)
    # Start recording, sending the output to the connection for 60
    # seconds, then stop
    output = StreamingOutput()
    camera.start_recording(output, format='mjpeg') # h264 ?
    while True:
      try:
        client_socket = socket.socket()
        client_socket.connect((config.values["value_relay_server"], int(config.values["boat_stream_port"])))
        pprint.pprint("Connected")
        connection = client_socket.makefile('wb')
        while True:
          with output.condition:
            output.condition.wait()
            frame = output.frame
          if frame != None:
            connection.write("frame,".encode("utf8") + len(frame).to_bytes(4, byteorder = 'big'))
            connection.write(frame)
            pprint.pprint("frame size: " + str(len(frame)))
      finally:
        pprint.pprint("bug")
    camera.stop_recording()
finally:
  client_socket.close()
