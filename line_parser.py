#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import logging
import pprint

class LineParser:
  partial_packet = b''
  logger = logging.getLogger("LineParser")

  def add_buffer(self, buffer):
    result = []
    self.partial_packet += buffer
    while True:
      try:
        eol_index = self.partial_packet.index(b'\n')
      except:
        eol_index = -1
      if eol_index == -1:
        break
      end_of_buffer_index = eol_index
      if eol_index > 0 and self.partial_packet[eol_index - 1] == 13:
        end_of_buffer_index -= 1
      full_packet = self.partial_packet[:end_of_buffer_index]
      self.partial_packet = self.partial_packet[eol_index + 1:]
      if len(full_packet) > 0:
        result.append(full_packet)
    return result

if __name__ == '__main__':
  line_parser = LineParser()
  list = line_parser.add_buffer(b'toto\r\ntata')
  pprint.pprint(list)
  list = line_parser.add_buffer(b'toto\n\rtata')
  pprint.pprint(list)
  list = line_parser.add_buffer(b'x\ny\n')
  pprint.pprint(list)
