#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Boat

import asyncio
import json
import logging
import os
import pprint
import sys

os.chdir(os.path.abspath(os.path.dirname(sys.argv[0])))
sys.path.append("..")

import config
import e3372
import value_sender

SETTINGS_DIR = "/etc/ulysse314/"
CONFIG_FILE = os.path.join(SETTINGS_DIR, "ulysse314.ini")
if len(sys.argv) == 1:
  with open(os.path.join(SETTINGS_DIR, "name"), "r") as file:
    BOAT_NAME = file.readline().strip()
else:
  BOAT_NAME = sys.argv[1]

logging.basicConfig(level=logging.DEBUG)
config.load(BOAT_NAME)

BOAT_PORT = int(config.values["boat_port"])
RELAY_SERVER = config.values["value_relay_server"]

sender = value_sender.ValueSender(None, BOAT_NAME, RELAY_SERVER, BOAT_PORT, config.values["boat_key"])

controllers = [ e3372.HuaweiE3372() ]
started_controllers = []
for controller in controllers:
  try:
    controller.start()
    logging.debug("{} started".format(controller.__class__.__name__))
    started_controllers.append(controller)
  except:
    logging.exception("{} not started !!!!".format(controller.__class__.__name__))

loop = asyncio.get_event_loop()
loop.create_task(sender.connect())
loop.run_forever()
