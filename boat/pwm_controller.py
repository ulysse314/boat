#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import Adafruit_PCA9685
import asyncio
import logging
import pprint

parent_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "../shared"))
if parent_dir not in sys.path:
  sys.path.append(parent_dir)

PWM_FORWARD_US = 0.0019
PWM_STOPPED_US = 0.0015
PWM_REVERSE_US = 0.0011
ADAFRUIT_FREQUENCE = 250
REAL_FREQUENCE = 297.7
LEFT_MOTOR_ID = 0
RIGHT_MOTOR_ID = 1

LEFT_LED_ID = 14
RIGHT_LED_ID = 15

def valid_motor(value):
  if value > 100.0:
    value = 100.0
  elif value < -100.0:
    value = -100.0
  return value

def valid_led(value):
  if value > 100.0:
    value = 100.0
  elif value < 0.0:
    value = 0.0
  return value

class PWMController:
  def __init__(self):
    self.logger = logging.getLogger(self.__class__.__name__)
    self.pwm = None
    self.values = {}
    self.motor_forward = PWM_FORWARD_US * 4096.0 * REAL_FREQUENCE
    self.motor_stopped = PWM_STOPPED_US * 4096.0 * REAL_FREQUENCE
    self.motor_reverse = PWM_REVERSE_US * 4096.0 * REAL_FREQUENCE
    self.left_motor = 0
    self.right_motor = 0
    self.left_led = 0
    self.right_led = 0
    try:
      self.pwm = Adafruit_PCA9685.PCA9685()
      self.pwm.set_pwm_freq(ADAFRUIT_FREQUENCE)
      self.set_left_led(0)
      self.set_right_led(0)
      self.set_left_motor(0)
      self.set_right_motor(0)
    except:
      self.logger.exception("Cannot init")

  def start(self):
    pass

  def start_update_values(self):
    pass

  def stop_update_values(self):
    self.values = { "motor": {"left%": self.left_motor, "right%": self.right_motor }, "led": {"left%": self.left_led, "right%": self.right_led } }

  def set_motors(self, values):
    if "left%" in values:
      self.set_left_motor(values["left%"])
    if "right%" in values:
      self.set_right_motor(values["right%"])

  def set_left_motor(self, value):
    value = valid_motor(value)
    self.left_motor = value
    self.set_motor(LEFT_MOTOR_ID, self.left_motor)

  def set_right_motor(self, value):
    value = valid_motor(value)
    self.right_motor = value
    self.set_motor(RIGHT_MOTOR_ID, self.right_motor)

  def set_motor(self, motor_id, value):
    if value > 0.0:
      real_value = self.motor_stopped + (self.motor_forward - self.motor_stopped) * value / 100.0
    elif value < 0.0:
      real_value = self.motor_stopped + (self.motor_stopped - self.motor_reverse) * value / 100.0
    else:
      real_value = self.motor_stopped
    if not self.pwm:
      self.logger.error("No pwm")
      return
    try:
      self.pwm.set_pwm(motor_id, 0, int(real_value))
    except:
      self.logger.exception("Cannot set motor id {} to value {}".format(motor_id, value))

  def set_leds(self, values):
    if "left%" in values:
      self.set_left_led(values["left%"])
    if "right%" in values:
      self.set_right_led(values["right%"])

  def set_left_led(self, value):
    value = valid_led(value)
    self.left_led = value
    self.set_led(LEFT_LED_ID, value)

  def set_right_led(self, value):
    value = valid_led(value)
    self.right_led = value
    self.set_led(RIGHT_LED_ID, value)

  def set_led(self, led_id, value):
    if not self.pwm:
      self.logger.error("No pwm")
      return
    try:
      self.pwm.set_pwm(led_id, int(value * 4096.0 / 100.0), 0)
    except:
      self.logger.exception("Cannot set led id {} to value {}".format(led_id, value))

async def debug(pwm_controller):
  while True:
    pprint.pprint(pwm_controller.values)
    await asyncio.sleep(1)

def main():
  pwm_controller = PWMController()
  pwm_controller.start()
  task = asyncio.ensure_future(debug(pwm_controller))
  loop = asyncio.get_event_loop()
  loop.run_forever()

if __name__ == "__main__":
  main()
