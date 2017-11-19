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
import value_sender

with open("/etc/ulysse314/name", "r") as file:
  BOAT_NAME = file.readline().strip()
CONFIG_FILE = "/etc/ulysse314/ulysse314.ini"

logging.basicConfig(level=logging.DEBUG)
config.load(CONFIG_FILE, BOAT_NAME)

BOAT_PORT = int(config.values["boat_port"])
RELAY_SERVER = config.value["value_relay_server"]

sender = value_sender.ValueSender(BOAT_NAME, RELAY_SERVER, BOAT_PORT, config.values["boat_key"])

loop = asyncio.get_event_loop()
loop.create_task(sender.run())
loop.run_forever()
