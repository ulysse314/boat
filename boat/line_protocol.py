import asyncio
import json
import logging
import pprint
import queue

class LineProtocol(asyncio.Protocol):
  partial_packet = b''

  def __init__(self, delegate):
    self.delegate = delegate
    self.logger = logging.getLogger(self.__class__.__name__)

# Notifications
  def connection_made(self, transport):
    self.transport = transport
    peer_name = self.transport.get_extra_info('peername')
    self.logger.info("Connection made from {}".format(peer_name))
    if self.delegate:
      self.delegate.connection_made(self)

  def connection_lost(self, ex):
    self.logger.info("Connection lost")
    if self.delegate:
      self.delegate.connection_lost(ex)

  def data_received(self, data):
    self.partial_packet += data
    while True:
      try:
        eol_index = self.partial_packet.index(b'\n')
      except:
        return
      if eol_index == -1:
        return
      end_of_buffer_index = eol_index
      if eol_index > 0 and self.partial_packet[eol_index - 1] == 13:
        end_of_buffer_index -= 1
      full_packet = self.partial_packet[:end_of_buffer_index]
      self.partial_packet = self.partial_packet[eol_index + 1:]
      if len(full_packet) > 0:
        line = full_packet.decode("utf-8")
        self.line_ready(line)

  def eof_received(self):
    if self.delegate:
      self.delegate.eof_received()

  def line_ready(self, line):
    if self.delegate:
      self.delegate.received_message(line)

# Actions
  def close(self):
    self.logger.info("Close")
    self.transport.write_eof()

  def send_line(self, data):
    self.transport.write(data.encode("utf-8"))
    self.transport.write("\n".encode("utf-8"))

  def get_extra_info(self, key):
    return self.transport.get_extra_info(key)
