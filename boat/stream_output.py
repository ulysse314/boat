#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Boat

import asyncio
import io
import json
import logging
import os
import picamera
import pprint
import socket
import sys
import time
from threading import Condition

parent_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "../shared"))
if parent_dir not in sys.path:
  sys.path.append(parent_dir)

import config

if len(sys.argv) <= 2:
  if len(sys.argv) == 1:
    BOAT_NAME = config.default_boat_name()
  else:
    BOAT_NAME = sys.argv[1]
  config.load(BOAT_NAME)
  SERVER_IP = config.values["value_relay_server"]
  SERVER_PORT = int(config.values["boat_stream_port"])
elif len(sys.argv) == 3:
  SERVER_IP = argv[1]
  SERVER_PORT = int(argv[2])

logging.basicConfig(level=logging.DEBUG)

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

pprint.pprint((SERVER_IP, SERVER_PORT))
try:
  with picamera.PiCamera() as camera:
    # 1296, 972
    # 640, 480
    camera.resolution = (1296, 972)
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
        client_socket.connect((SERVER_IP, SERVER_PORT))
        pprint.pprint("Connected")
        while True:
          with output.condition:
            output.condition.wait()
            frame = output.frame
          frame_size = len(frame)
          if frame != None and frame_size > 0:
#            pprint.pprint("send: " + str(frame_size))
            client_socket.sendall("frame,".encode("utf8") + frame_size.to_bytes(4, byteorder = 'big'))
            client_socket.sendall(frame)
            data = client_socket.recv(1)
      finally:
        pprint.pprint("bug")
    camera.stop_recording()
finally:
  client_socket.close()
