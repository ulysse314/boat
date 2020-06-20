#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import asyncio
import json
import logging
import os
import pprint
import queue
import sys

parent_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "../shared"))
if parent_dir not in sys.path:
  sys.path.append(parent_dir)

import line_parser

class LineProtocol(asyncio.Protocol):

  def __init__(self, delegate):
    self.delegate = delegate
    self.logger = logging.getLogger(self.__class__.__name__)
    self.line_parser = line_parser.LineParser()

# Notifications
  def connection_made(self, transport):
    self.transport = transport
    peer_name = self.transport.get_extra_info('peername')
    self.logger.info("Connection made from {}".format(peer_name))
    if self.delegate:
      self.delegate.connection_made(self)

  def connection_lost(self, ex):
    self.logger.info("Connection lost")
    if self.delegate:
      self.delegate.connection_lost(self)

  def data_received(self, data):
    list = self.line_parser.add_buffer(data)
    for line in list:
      line = line.decode("utf-8")
      self.line_ready(line)

  def eof_received(self):
    if self.delegate:
      self.delegate.eof_received()

  def line_ready(self, line):
    if self.delegate:
      self.delegate.received_message(line)

  def pause_writing(self):
    self.logger.warning("pause writting !!!!")

  def resume_writing(self):
    self.logger.warning("resume writting !!!!")

# Actions
  def close(self):
    self.logger.info("Close")
    self.transport.write_eof()

  def send_line(self, line):
    self.transport.write(line.encode("utf-8"))
    self.transport.write("\n".encode("utf-8"))

  def send_lines(self, lines):
    for line in lines:
      self.send_line(line)

  def get_extra_info(self, key):
    return self.transport.get_extra_info(key)
