#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import json
import os
import pprint
import sys

SETTINGS_DIR = "/etc/ulysse314/"
values = None

def default_boat_name():
  global SETTINGS_DIR
  with open(os.path.join(SETTINGS_DIR, "name"), "r") as file:
    boat_name = file.readline().strip()
  return boat_name

def load(boat_name = None):
  global values
  global SETTINGS_DIR
  if not boat_name:
    boat_name = default_boat_name()
  if values:
    return values
  values = {}
  json_file = os.path.join(SETTINGS_DIR, "ulysse314.json")
  with open(json_file) as data_file:
    loaded_json = json.load(data_file)
    for key in loaded_json["shared"]:
      values[key] = loaded_json["shared"][key]
    for key in loaded_json["boats"][boat_name]:
      values[key] = loaded_json["boats"][boat_name][key]
  return values
