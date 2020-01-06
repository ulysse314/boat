#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import asyncio
import datetime
import logging
import pprint
import time

import munin_server
import value_sender

class BoatController:
  def __init__(self, controllers, arduino, commnand, value_sender):
    self.logger = logging.getLogger(self.__class__.__name__)
    self.record = False
    self.boot_timestamp = time.time()
    self.value_id = 0
    self.controllers = controllers
    self.arduino_controller = arduino
    self.command_controller = commnand
    self.value_sender = value_sender

  def _get_values(self):
    values = {}
    values["id"] = self.value_id
    self.value_id += 1
    values["record"] = self.record
    i = datetime.datetime.now()
    values['dt'] = i.isoformat()
    values["tmstmp"] = time.time()
    values["bttmstmp"] = self.boot_timestamp
    for controller in self.controllers:
      try:
        controller.stop_update_values()
        controller_values = controller.values
        for key in controller_values:
          if key not in values:
            values[key] = controller_values[key]
          else:
            values[key].update(controller_values[key])
        controller.start_update_values()
      except:
        self.logger.exception("Problem to get values with {}".format(pprint.pformat(controller)))
    return values
  
  def start(self):  
    asyncio.ensure_future(self._run())

  async def _run(self):
    self._turn_off_motors()
    self.received_values({"led":{"left%":100,"right%":0}})
    await asyncio.sleep(0.25)
    self.received_values({"led":{"left%":0,"right%":100}})
    await asyncio.sleep(0.25)
    self.received_values({"led":{"left%":100,"right%":0}})
    await asyncio.sleep(0.25)
    self.received_values({"led":{"left%":0,"right%":100}})
    await asyncio.sleep(0.25)
    self.received_values({"led":{"left%":0,"right%":0}})
    while True:
      values = self._get_values()
      munin_server.set_values(values)
      self.value_sender.add_values(values)
      await asyncio.sleep(1)

  def _turn_off_motors(self):
    self.received_values({"motor":{"left%":0,"right%":0}})

## value_sender delegate
  def received_values(self, values):
    try:
      for key in values:
        value = values[key]
        if key == "record":
          self.record = value
        elif key == "motor" and type(value) is dict:
          self.arduino_controller.set_motors(value)
        elif key == "led" and type(value) is dict:
          self.arduino_controller.set_leds(value)
        elif key == "controller" and value == 0:
          self._turn_off_motors()
        elif key == "command":
          self.command_controller.execute(value)
        elif key == "ping":
          self.arduino_controller.send_ping()
        else:
          self.logger.warning("Command unknown")
          self.logger.warning(pprint.pformat(values))
    except Exception as e:
      self.logger.exception("set values")

  def connection_lost(self):
    self._turn_off_motors()
