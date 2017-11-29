#!/usr/bin/env python3

import aiohttp
import asyncio
import logging
import pprint
import sys
import time
import threading
import xmltodict

class HuaweiE3372:
  BASE_URL = 'http://{host}'
  COOKIE_URL = '/html/index.html'
  XML_APIS = [
    '/api/device/information',
    '/api/device/signal',
    '/api/monitoring/status',
    '/api/monitoring/traffic-statistics',
    '/api/dialup/connection',
    '/api/global/module-switch',
    '/api/net/current-plmn',
    '/api/net/net-mode',
  ]
  KEYS = [
    "ConnectionStatus",
    "CurrentNetworkType",
    "CurrentNetworkTypeEx",
    "CurrentServiceDomain",
    "ServiceStatus",
    "SignalIcon",
    "SignalStrength",
    "SimStatus",
    "cell_id",
    "cellroam",
    "classify",
    "ecio",
    "flymode",
    "maxsignal",
    "mode",
    "pci",
    "rscp",
    "rsrp",
    "rsrq",
    "rssi",
    "sc",
    "simlockStatus",
    "sinr",
  ]
  session = None

  def __init__(self, host = '192.168.8.1'):
    self.logger = logging.getLogger('e3372')
    self.logger.debug("e3372 init start")
    threading.Thread.__init__(self)
    self.host = host
    self.base_url = self.BASE_URL.format(host=host)
    self.values = {}
    self.running = True #setting the thread running to true
    self.session = None
    self.logger.debug("e3372 init stop")

  def start(self):
    task = asyncio.ensure_future(self.run())

  async def get(self,path):
    try:
      if self.session == None:
        self.session = aiohttp.ClientSession()
      # get a session cookie by requesting the COOKIE_URL
      await self.session.get(self.base_url + self.COOKIE_URL, timeout = 10)
      return xmltodict.parse(self.session.get(self.base_url + path, timeout = 5).text).get('response',None)
    except Exception as e:
      self.logger.exception("get")
      self.session = None
    return {}

  async def run(self):
    while self.running:
      values = {}
      try:
        tasks = [ self.get('/api/device/signal'), self.get('/api/monitoring/status')]
        for finished in asyncio.as_completed(tasks):
          dict = await finished
          for key,value in dict.items():
            if key in self.KEYS:
              values[key] = value
      except Exception as e:
        self.logger.exception("run")
      self.values = values
      await asyncio.sleep(1)

#def main():
#  e3372 = HuaweiE3372()
#  for path in e3372.XML_APIS:
#    for key,value in e3372.get(path).items():
#      print(key,value)
#
#if __name__ == "__main__":
#  main()
