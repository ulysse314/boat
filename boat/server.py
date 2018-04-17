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

import arduino_controller
import boat_controller
import config
import e3372_controller
import gps_controller
import munin_server
import pi_controller
import pwm_controller
import value_sender

if len(sys.argv) == 1:
  BOAT_NAME = config.default_boat_name()
else:
  BOAT_NAME = sys.argv[1]

logging.basicConfig(level=logging.DEBUG)
config.load(BOAT_NAME)

BOAT_PORT = int(config.values["boat_port"])
RELAY_SERVER = config.values["value_relay_server"]

sender = value_sender.ValueSender(BOAT_NAME, RELAY_SERVER, BOAT_PORT, config.values["boat_key"])

pwm = pwm_controller.PWMController()
controllers = [ e3372_controller.E3372Controller(),
                gps_controller.GPSController(),
                pi_controller.PiController(),
                pwm,
                arduino_controller.ArduinoController() ]
started_controllers = []
for controller in controllers:
  try:
    controller.start()
    logging.debug("{} started".format(controller.__class__.__name__))
    started_controllers.append(controller)
  except:
    logging.exception("{} not started !!!!".format(controller.__class__.__name__))

boat = boat_controller.BoatController(started_controllers, pwm, sender)
sender.delegate = boat
boat.start()
sender.start()
munin_server.start(int(config.values["munin_port"]))

loop = asyncio.get_event_loop()
loop.run_forever()
