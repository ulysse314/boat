import asyncio
import json
import logging
import pprint
import queue

import line_protocol

class JsonProtocol(line_protocol.LineProtocol):
  def line_ready(self, line):
    try:
      message = json.loads(line)
    except:
      self.logger.exception("read value {}".format(pprint.pformat(string)))
      self.transport.close()
    if self.delegate:
      self.delegate.received_message(message)

  def send_json(self, values):
    data = json.dumps(values)
    self.send_line(data.encode("utf-8"))
