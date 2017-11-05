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

import camera
import config
import value_logger

BOAT_NAME = sys.argv[1]
CONFIG_FILE = sys.argv[2]

logging.basicConfig(level=logging.DEBUG)
config.load(CONFIG_FILE, BOAT_NAME)

BOAT_PORT = int(config.values["boat_port"])
