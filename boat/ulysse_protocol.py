import asyncio
import json
import logging
import queue

class UlysseProtocol(asyncio.Protocol):
  partial_packet = b''

  def __init__(self, delegate):
    self.delegate = delegate
    self.logger = logging.getLogger("UlysseProtocol")

# Notifications
  def connection_made(self, transport):
    self.logger.info("Connection made")
    self.transport = transport
    self.delegate.connection_made(self)

  def connection_lost(self, ex):
    self.logger.info("Connection lost")
    self.delegate.connection_lost(self, ex)

  def data_received(self, data):
    eol_index = data.index(b'\n')
    if eol_index == -1:
      self.partial_packet += data
      return
    end_of_buffer_index = eol_index
    if eol_index > 0 and data[eol_index] == b'\r':
      end_of_buffer_index -= 1
    full_packet = self.partial_packet + data[:end_of_buffer_index - 1]
    if len(full_packet) == 0:
      return
    self.partial_packet = data[eol_index + 1:]
    try:
      message = json.loads(full_packet.decode("utf-8"))
    except:
      self.logger.exception("read value")
      pprint.pprint(data)
      self.transport.close()
    self.delegate.received_message(message)

  def eof_received(self):
    self.logger.info("EOF received")
    self.close()

# Actions
  def close(self):
    self.logger.info("Close")
    self.transport.write_eof()

  def send_key(self, data):
    self.transport.write(data)
    self.transport.write("\n")

  def send_values(self, values):
    data = json.dumps(values)
    self.transport.write(data)
    self.transport.write("\n")
