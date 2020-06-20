#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import logging
import os
import pprint
import signal
import subprocess
import sys
import threading

parent_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "../shared"))
if parent_dir not in sys.path:
  sys.path.append(parent_dir)

import process_controller

class CameraController:
  def __init__(self, boat_name):
    self.logger = logging.getLogger(self.__class__.__name__)
    self.boat_name = boat_name
    self.camera_process = None

  def start(self):
    self.logger.debug("Start camera")
    camera_process_path = os.path.abspath(os.path.join(os.path.dirname(__file__), "stream_relay.py"))
    self.camera_process = process_controller.ProcessController(camera_process_path, [ self.boat_name ])
    self.camera_process.start()

  def stop(self):
    if self.camera_process == None:
      self.logger.debug("Camera not started")
      return
    self.logger.debug("Stop camera")
    self.camera_process.stop()
    self.camera_process = None
