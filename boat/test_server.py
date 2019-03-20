#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Boat

import asyncio
import json
import logging
import os
import pprint
import sys

parent_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
if parent_dir not in sys.path:
  sys.path.append(parent_dir)

import boat_controller
import command_controller
import config
import e3372_controller
import feather_controller
import gps_controller
import pi_controller
import pwm_controller
import value_sender

BOAT_NAME = "test"

logging.basicConfig(level=logging.DEBUG)
config.load(BOAT_NAME)

BOAT_PORT = int(config.values["boat_port"])
RELAY_SERVER = config.values["value_relay_server"]

sender = value_sender.ValueSender(BOAT_NAME, RELAY_SERVER, BOAT_PORT, config.values["boat_key"])

if sys.argv[1] == "feather":
  controller = feather_controller.FeatherController(config.values["sensors"])
elif sys.argv[1] == "e3372":
  controller = e3372_controller.E3372Controller()
elif sys.argv[1] == "pi":
  controller = pi_controller.PiController()
else:
  controller = None

try:
  controller.start()
  logging.debug("{} started".format(controller.__class__.__name__))
except:
  logging.exception("{} not started !!!!".format(controller.__class__.__name__))
  exit()

async def run_controller(controller):
  while True:
    controller.stop_update_values()
    print("-----------------------")
    pprint.pprint(controller.values)
    controller.start_update_values()
    await asyncio.sleep(1)

asyncio.ensure_future(run_controller(controller))
loop = asyncio.get_event_loop()
loop.run_forever()
