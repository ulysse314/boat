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

BOAT_NAME = sys.argv[1]
CONFIG_FILE = sys.argv[2]

logging.basicConfig(level=logging.DEBUG)
config.load(CONFIG_FILE, BOAT_NAME)

BOAT_PORT = int(config.values["boat_port"])
RELAY_SERVER = config.value["value_relay_server"]

sender = value_sender.ValueSender(BOAT_NAME, RELAY_SERVER, BOAT_PORT, config.values["boat_key"])

loop = asyncio.get_event_loop()
loop.create_task(sender.run())
loop.run_forever()
