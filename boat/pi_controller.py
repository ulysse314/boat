#!/usr/bin/env python3

import asyncio
import os
import pprint
import psutil
import subprocess

class PiController:
  values = {}

  def start(self):
    asyncio.ensure_future(self.run())

  async def run(self):
    while True:
      values = await self._get_values()
      self.values["pi"] = values
      await asyncio.sleep(1)

  async def _get_values(self):
    return { "temp": self._get_cpu_temperature(), "ram": self._get_ram_info(), "cpu%": psutil.cpu_percent(), "disk": self._get_disk_space() }

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
    return { "total": ram.total / unit, "used": ram.used / unit, "free": ram.free / unit, "used%": ram.percent }

  # Return % of CPU used by user as a character string                                
  def _get_cpu_use(self):
    return(str(os.popen("top -n1 | awk '/Cpu\(s\):/ {print $2}'").readline().strip()))

  # Return information about disk space as a list (unit included)                     
  def _get_disk_space(self):
    disk = psutil.disk_usage('/')
    unit = 2**20
    return { "size": disk.total / unit, "used": disk.used / unit, "available": disk.free / unit, "used%": disk.percent }

async def debug(pi_controller):
  while True:
    pprint.pprint(pi_controller.values)
    await asyncio.sleep(1)

def main():
  pi_controller = PiController()
  pi_controller.start()
  task = asyncio.ensure_future(debug(pi_controller))
  loop = asyncio.get_event_loop()
  loop.run_forever()

if __name__ == "__main__":
  main()
