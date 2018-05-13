#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# ./e3372_controller.py [router_ip_optional]

import aiohttp
import asyncio
import logging
import pprint
import sys
import time
import xmltodict

class E3372Controller:
  BASE_URL = 'http://{host}{url}'
  COOKIE_URL = '/html/index.html'
  APIS = [ '/api/device/signal',
    '/api/monitoring/status',
    '/api/monitoring/traffic-statistics' ]
  APIS_AVAILABLE = [
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
    # /api/monitoring/status
    "ConnectionStatus",
    "CurrentNetworkType",
    "CurrentNetworkTypeEx",
    "CurrentServiceDomain",
    "ServiceStatus",
    "SignalIcon",
    "SignalStrength",
    "SimStatus",
    "cellroam",
    "classify",
    "flymode",
    "maxsignal",
    "simlockStatus",
    # /api/device/signal
    "cell_id",
    "ecio",
    "mode",
    "pci",
    "rscp",
    "rsrp",
    "rsrq",
    "rssi",
    "sc",
    "sinr",
    # /api/monitoring/traffic-statistics
    "CurrentConnectTime",
    "CurrentUpload",
    "CurrentDownload",
    "CurrentDownloadRate",
    "CurrentUploadRate",
  ]
  ROUTER_IP = '192.168.8.1'

  def __init__(self, router_ip = None):
    self.session = None
    self.logger = logging.getLogger(self.__class__.__name__)
    self.values = {}
    if router_ip:
      self.router_ip = router_ip
    else:
      self.router_ip = self.ROUTER_IP
    self.base_url = self.BASE_URL.format(host = self.router_ip, url = "{url}")

  def start(self):
    asyncio.ensure_future(self._run())

  def start_update_values(self):
    pass

  def stop_update_values(self):
    pass

  async def _run(self):
    while True:
      values = await self._get_values()
      self.values = { "cellular": values }
      await asyncio.sleep(1)

  async def _get_values(self):
    values = {}
    try:
      for api in self.APIS:
        dict = await self._get(api)
        for key,value in dict.items():
          if key in self.KEYS:
            values[key] = value
    except Exception as e:
      self.logger.exception("run")
    return values

  async def _get(self, path):
    try:
      if self.session == None:
        # cookie jar should be unsafe since the domain is an ip.
        self.session = aiohttp.ClientSession(cookie_jar = aiohttp.CookieJar(unsafe = True))
        resp = await self.session.get(self.base_url.format(url = self.COOKIE_URL), timeout = 10)
        if not resp.cookies["SessionID"]:
          self.logger.warning("No session cookie")
          self.session = None
      # get a session cookie by requesting the COOKIE_URL
      url = self.base_url.format(url = path)
      self.logger.debug(url)
      resp = await self.session.get(url, timeout = 10)
      if resp.status != 200:
        self.logger.warning("Error with URL " + url)
        return {}
      text = await resp.text()
      json = xmltodict.parse(text)
      if "error" in json:
        self.logger.warning("Error")
        self.logger.warning(pprint.pformat(json))
        self.session = None
        return {}
      return json["response"]
    except Exception as e:
      self.logger.exception("get")
      self.session = None
    return {}

async def debug(e3372_controller):
  while True:
    values = await e3372_controller._get_values()
    pprint.pprint(values)
    print("====================================")
    for api in e3372_controller.APIS_AVAILABLE:
      pprint.pprint(api)
      values = await e3372_controller._get(api)
      pprint.pprint(values)
      print(" ")
    print("------------------------------------")
    await asyncio.sleep(1)

def main():
  router_ip = None
  if len(sys.argv) >= 2:
    router_ip = sys.argv[1]
  e3372_controller = E3372Controller(router_ip)
  task = asyncio.ensure_future(debug(e3372_controller))
  loop = asyncio.get_event_loop()
  loop.run_forever()

if __name__ == "__main__":
  main()
