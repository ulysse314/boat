#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import asyncio
import logging
import pprint
import subprocess
import sys

class ProcessController:
  def __init__(self, process_command, parameters):
    self.logger = logging.getLogger(self.__class__.__name__)
    self.process_command = process_command
    self.parameters = parameters
    self.process = None

## controller methods
  def start(self):
    if self.process != None:
      return
    asyncio.ensure_future(self._start_process())

  def stop(self):
    process = self.process
    self.process = None
    process.terminate()

## Private
  async def _start_process(self):
    all_parameters = [ self.process_command ] + self.parameters
    counter = 0
    while True:
      self.process = await asyncio.create_subprocess_exec(
          *all_parameters
          #, stdout=asyncio.subprocess.PIPE, stderr=asyncio.subprocess.PIPE
      )
      stdout, stderr = await self.process.communicate()
      if self.process == None:
        break
      counter += 1
      await asyncio.sleep(2)

def main():
  parameters = []
  if len(sys.argv) > 2:
    parameters = sys.argv[2:]
  process_controller = ProcessController(sys.argv[1], parameters)
  process_controller.start()
  loop = asyncio.get_event_loop()
  loop.run_forever()

if __name__ == "__main__":
  main()
