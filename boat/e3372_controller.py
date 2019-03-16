#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# ./e3372_controller.py [router_ip_optional]

import aiohttp
import asyncio
import logging
import os
import pprint
import sys
import time
import xmltodict

parent_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
if parent_dir not in sys.path:
  sys.path.append(parent_dir)

import error

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
    #"SignalStrength",
    "SimStatus",
    "cellroam",
    "classify",
    "flymode",
    "maxsignal",
    "simlockStatus",
    # /api/device/signal
    #"cell_id",
    #"ecio",
    "mode",
    #"pci",
    #"rscp",
    #"rsrp",
    #"rsrq",
    #"rssi",
    #"sc",
    #"sinr",
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
    self.errors = {}
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
      (errors, values) = await self._get_values()
      merged_values = {}
      merged_values.update(values)
      merged_values.update({"errors": errors})
      self.values = { "cellular": merged_values }
      await asyncio.sleep(1)

  async def _get_values(self):
    all_errors = []
    values = {}
    try:
      for api in self.APIS:
        errors, dict = await self._get(api)
        if errors:
          all_errors.extend(errors)
        if dict != None:
          for key,value in dict.items():
            if key in self.KEYS:
              values[key] = value
    except Exception as e:
      self.logger.exception("run")
      all_errors.append(error.json_error(error = error.E3372Error_UnknownGetValues))
    return (all_errors, values)

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
        error_message = "Error with URL " + url + ", status: " + str(resp.status)
        self.logger.warning(error_message)
        return ([error.json_error(error = error.E3372Error_Status, message = error_message)], None)
      text = await resp.text()
      json = xmltodict.parse(text)
      if "error" in json:
        error_message = "Error with URL " + url + ", json received: " + pprint.pformat(json)
        self.logger.warning(error_message)
        self.session = None
        return ([error.json_error(error = error.E3372Error_Json, message = error_message)], None)
      return (None, json["response"])
      
    except Exception as e:
      self.session = None
      error_message = "Error with path " + path + ", " + pprint.pformat(e)
      self.logger.exception(error_message)
      return ([error.json_error(error = error.E3372Error_UnknownGet, message = error_message)], None)
    error_message = "Error with path " + path
    return ([error.json_error(error = error.E3372Error_UnknownGet, message = error_message)], None)

async def test_apis(e3372_controller):
  for api in e3372_controller.APIS_AVAILABLE:
    (errors, values) = await e3372_controller._get(api)
    print("==================================== " + api)
    pprint.pprint(values)
    pprint.pprint(errors)
    print(" ")

async def test_controller(e3372_controller):
  while True:
    (errors, values) = await e3372_controller._get_values()
    print("------------------------------------")
    pprint.pprint(values)
    pprint.pprint(errors)
    print(" ")
    await asyncio.sleep(1)

def main():
  router_ip = None
  if len(sys.argv) >= 2:
    router_ip = sys.argv[1]
  e3372_controller = E3372Controller(router_ip)
  e3372_controller.start()
  asyncio.ensure_future(test_apis(e3372_controller))
  asyncio.ensure_future(test_controller(e3372_controller))
  loop = asyncio.get_event_loop()
  loop.run_forever()

if __name__ == "__main__":
  main()
