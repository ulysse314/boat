#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import asyncio
import logging
import os
import pprint
import psutil
import subprocess
import sys

parent_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "../shared"))
if parent_dir not in sys.path:
  sys.path.append(parent_dir)

import boat_error

class PiController:
  def __init__(self):
    self.values = {}
    self.logger = logging.getLogger(self.__class__.__name__)
    self.loop_duration = 0

  def start(self):
    pass

  def start_update_values(self):
    asyncio.ensure_future(self._get_values())

  def stop_update_values(self):
    pass

  async def _get_values(self):
    try:
      self.values["pi"] = { "temp": self._get_cpu_temperature(), "cpu%": psutil.cpu_percent() }
      self.values["pi"]["lpdrt"] = self.loop_duration * 1000
      self.values["pi"].update(self._get_disk_space())
      self.values["pi"].update(self._get_ram_info())
      errors = []
      if self.values["pi"]["temp"] > 90:
        errors.append([boat_error.PiDomain, boat_error.Pi.temperatureCritical, str(self.values["pi"]["temp"]) ])
      elif self.values["pi"]["temp"] > 75:
        errors.append([boat_error.PiDomain, boat_error.Pi.temperatureWarning, str(self.values["pi"]["temp"]) ])
      elif self.values["pi"]["temp"] > 60:
        errors.append([boat_error.PiDomain, boat_error.Pi.temperatureInfo, str(self.values["pi"]["temp"]) ])
      if self.values["pi"]["cpu%"] > 90:
        errors.append([boat_error.PiDomain, boat_error.Pi.cpuCritical, str(self.values["pi"]["cpu%"]) ])
      elif self.values["pi"]["cpu%"] > 70:
        errors.append([boat_error.PiDomain, boat_error.Pi.cpuWarning, str(self.values["pi"]["cpu%"]) ])
      elif self.values["pi"]["cpu%"] > 50:
        errors.append([boat_error.PiDomain, boat_error.Pi.cpuInfo, str(self.values["pi"]["cpu%"]) ])
        self.saveProcesses()
      if self.values["pi"]["ram.used%"] > 90:
        errors.append([boat_error.PiDomain, boat_error.Pi.memoryCritical, str(self.values["pi"]["ram.used%"]) ])
      elif self.values["pi"]["ram.used%"] > 70:
        errors.append([boat_error.PiDomain, boat_error.Pi.memoryWarning, str(self.values["pi"]["ram.used%"]) ])
      elif self.values["pi"]["ram.used%"] > 50:
        errors.append([boat_error.PiDomain, boat_error.Pi.memoryInfo, str(self.values["pi"]["ram.used%"]) ])
      if self.values["pi"]["disk.used%"] > 90:
        errors.append([boat_error.PiDomain, boat_error.Pi.diskCritical, str(self.values["pi"]["disk.used%"]) ])
      elif self.values["pi"]["disk.used%"] > 70:
        errors.append([boat_error.PiDomain, boat_error.Pi.diskWarning, str(self.values["pi"]["disk.used%"]) ])
      elif self.values["pi"]["disk.used%"] > 50:
        errors.append([boat_error.PiDomain, boat_error.Pi.diskInfo, str(self.values["pi"]["disk.used%"]) ])
      if len(errors) > 0:
        self.values["pi"][boat_error.ErrorKey] = errors
    except:
      self.logger.exception("Get values")

  def saveProcesses(self):
    list = subprocess.Popen(['ps', 'auxww', '--sort=-pcpu'], stdout=subprocess.PIPE).communicate()[0]
    file = open("/tmp/processes.txt",'a+b')
    file.write(list)
    file.close()

  def _get_cpu_temperature(self):
    process = subprocess.Popen(['vcgencmd', 'measure_temp'], stdout=subprocess.PIPE)
    output, _error = process.communicate()
    output = output.decode("utf-8")
    return float(output[output.index('=') + 1:output.rindex("'")])

  # Return RAM information (unit=kb) in a list                                        
  # Index 0: total RAM                                                                
  # Index 1: used RAM                                                                 
  # Index 2: free RAM                                                                 
  def _get_ram_info(self):
    ram = psutil.virtual_memory()
    unit = 2**20
    return { "ram.total": ram.total / unit, "ram.used": ram.used / unit, "ram.free": ram.free / unit, "ram.used%": ram.percent }

  # Return % of CPU used by user as a character string                                
  def _get_cpu_use(self):
    return(str(os.popen("top -n1 | awk '/Cpu\(s\):/ {print $2}'").readline().strip()))

  # Return information about disk space as a list (unit included)                     
  def _get_disk_space(self):
    disk = psutil.disk_usage('/')
    unit = 2**20
    return { "disk.size": disk.total / unit, "disk.used": disk.used / unit, "disk.available": disk.free / unit, "disk.used%": disk.percent }

async def debug(pi_controller):
  while True:
    pi_controller.start_update_values()
    pi_controller.stop_update_values()
    await asyncio.sleep(1)
    pprint.pprint(pi_controller.values)

def main():
  pi_controller = PiController()
  pi_controller.start()
  task = asyncio.ensure_future(debug(pi_controller))
  loop = asyncio.get_event_loop()
  loop.run_forever()

if __name__ == "__main__":
  main()
