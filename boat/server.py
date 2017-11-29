#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Boat

import asyncio
import json
import logging
import os
import pprint
import sys

import e3372

os.chdir(os.path.abspath(os.path.dirname(sys.argv[0])))
sys.path.append("..")

import config
import value_sender

SETTINGS_DIR = "/etc/ulysse314/"
CONFIG_FILE = os.path.join(SETTINGS_DIR, "ulysse314.ini")
with open(os.path.join(SETTINGS_DIR, "name"), "r") as file:
  BOAT_NAME = file.readline().strip()

logging.basicConfig(level=logging.DEBUG)
config.load(CONFIG_FILE, BOAT_NAME)

BOAT_PORT = int(config.values["boat_port"])
RELAY_SERVER = config.values["value_relay_server"]

sender = value_sender.ValueSender(BOAT_NAME, RELAY_SERVER, BOAT_PORT, config.values["boat_key"])

controllers = [ e3372.HuaweiE3372() ]

loop = asyncio.get_event_loop()
loop.create_task(sender.run())
loop.run_forever()
