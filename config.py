import configparser
import pprint
import sys

values = None

def load(filename, boat_name):
  global values
  parser = configparser.SafeConfigParser()
  parser.read(filename)

  if values == None:
    values = {}
    for name, value in parser.items("shared"):
      values[name] = value
    for name, value in parser.items(boat_name):
      values[name] = value
  return values
