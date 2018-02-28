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
  def __init__(self, controllers, pwm, value_sender):
    self.logger = logging.getLogger("Boat");
    self.record = False
    self.boot_date = datetime.datetime.now().isoformat()
    self.value_id = 0
    self.controllers = controllers
    self.pwm_controller = pwm
    self.value_sender = value_sender

  def _get_values(self):
    values = {}
    values["id"] = self.value_id
    self.value_id += 1
    values["record"] = self.record
    i = datetime.datetime.now()
    values['date'] = i.isoformat()
    values["timestamp"] = time.time()
    values["boot_date"] = self.boot_date
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

## value_sender delegate
  def received_values(self, values):
    try:
      for key in values:
        value = values[key]
        if key == "record":
          self.record = values[key]
        elif key == "motor" and type(value) is dict:
          self.pwm_controller.set_motors(value)
        elif key == "led" and type(value) is dict:
          self.pwm_controller.set_leds(value)
    except Exception as e:
      self.logger.exception("set values")
