#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import asyncio
import json
import logging
import os
import sys

parent_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "../shared"))
if parent_dir not in sys.path:
  sys.path.append(parent_dir)

latest_values = b'{}'
logger = logging.getLogger("MuninServer")

def start(port):
  global logger
  logger.debug("start")
  loop = asyncio.get_event_loop()
  coro = loop.create_server(lambda: MuninServer(), port = port)
  loop.run_until_complete(coro)

def set_values(value):
  global latest_values
  string = json.dumps(value)
  latest_values = string.encode("utf-8")

class MuninServer(asyncio.Protocol):
  def __init__(self):
    pass

  def connection_made(self, transport):
    global latest_values
    global logger
    self.transport = transport
    self.peername = transport.get_extra_info("peername")
    logger.debug(self.peername)
    self.transport.write(latest_values)
    self.transport.write(b'\n')
    self.transport.close()

def main():
  start(1234)
  set_values({"a":1})
  asyncio.get_event_loop().run_forever()

if __name__ == '__main__':
  main()
