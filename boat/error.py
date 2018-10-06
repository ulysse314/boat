#!/usr/bin/env python3
# -*- coding: utf-8 -*-

E3372Error_Status = "e3372.status"
E3372Error_Json = "e3372.json"
E3372Error_UnknownGet = "e3372.unknownget"
E3372Error_UnknownGetValues = "e3372.unknowngetvalues"

def json_error(error, message = ""):
  return { "error": error, "message": message }

  