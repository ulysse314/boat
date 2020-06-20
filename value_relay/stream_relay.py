#!/usr/bin/env python3.8
# -*- coding: utf-8 -*-

import asyncio
import aiohttp
import aiohttp.web
import io
import pprint

client_queues = []
frame_data = []
my_boundary = "frame"

async def client_handler(request):
  global client_queues
  info = request.transport.get_extra_info('peername')
  print(f"{info!r} connected !!!!")
  response = aiohttp.web.StreamResponse(status=200, reason='OK', headers={
    'Content-Type': 'multipart/x-mixed-replace; '
                    'boundary=--%s' % my_boundary,
  })
  await response.prepare(request)
  while True:
    data_queue = asyncio.Queue()
    client_queues.append(data_queue)
    size = await data_queue.get()
    await response.write(b'--' + my_boundary.encode("utf8") + b'\r\n')
    await response.write(b'Content-Type: image/jpeg\r\n')
    await response.write(b'Content-Length: ' + str(size).encode("utf8") + b'\r\n\r\n')
    while True:
      data = await data_queue.get()
      if data == None:
        await response.write(b'\r\n')
        break
      await response.write(data)
  wc.shutdown()
  return response

async def send_to_clients(current_client_queues, data):
  for client_queue in current_client_queues:
    await client_queue.put(data)

async def stream_handler(reader, writer):
  global client_queues
  addr = writer.get_extra_info('peername')
  print(f"stream {addr!r} is connected !!!!")
  while True:
    data = b''
    while len(data) < 6:
      data += await reader.read(6 - len(data))
    if not data == b"frame,":
#      pprint.pprint(data)
      break;
    size_binary = await reader.read(4)
    expected_size = int.from_bytes(size_binary, "big")
    if expected_size == 0:
      continue
#    pprint.pprint("size :" + str(expected_size))
    current_client_queues = client_queues
    client_queues = []
    await send_to_clients(current_client_queues, expected_size)
    while expected_size > 0:
      data = await reader.read(expected_size)
      await send_to_clients(current_client_queues, data)
      expected_size -= len(data)
#      pprint.pprint("receive : " + str(len(data)) + " left: " + str(expected_size))
    writer.write(b'!')
    await send_to_clients(current_client_queues, None)
  writer.close()

def main():
  loop = asyncio.get_event_loop()
  # Create server for to receive stream
  stream_server = asyncio.start_server(stream_handler, '0.0.0.0', 31402)
  stream_server = loop.run_until_complete(stream_server)
  print('Serving on {}'.format(stream_server.sockets[0].getsockname()))

  # Create http server for clients
  app = aiohttp.web.Application()
  app.router.add_get('/', client_handler)
  client_server = loop.create_server(app.make_handler(), '0.0.0.0', 31412)
  client_server = loop.run_until_complete(client_server)
  print('Serving on {}'.format(client_server.sockets[0].getsockname()))

  loop.run_forever()

main()
