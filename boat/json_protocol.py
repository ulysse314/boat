#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import asyncio
import json
import logging
import os
import pprint
import queue
import sys

os.chdir(os.path.abspath(os.path.dirname(__file__)))

import line_protocol

class JsonProtocol(line_protocol.LineProtocol):
  def line_ready(self, line):
    try:
      message = json.loads(line)
      if self.delegate:
        self.delegate.received_message(message)
    except:
      self.logger.exception("read value {}".format(pprint.pformat(line)))
      self.transport.close()

  def send_json(self, values):
    data = json.dumps(values)
    self.send_line(data)
