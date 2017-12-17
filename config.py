import json
import pprint
import sys

values = None

def load(boat_name):
  global values
  if values:
    return values
  values = {}
  with open("/etc/ulysse314/ulysse314.json") as data_file:
    loaded_json = json.load(data_file)
    for key in loaded_json["shared"]:
      values[key] = loaded_json["shared"][key]
    for key in loaded_json["boats"][boat_name]:
      values[key] = loaded_json["boats"][boat_name][key]
  return values
