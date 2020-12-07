#!/usr/bin/env python3.8
# -*- coding: utf-8 -*-

# ./stream_relay.py boat_name live_stream_name
# ./stream_relay.py boat_stream_port client_stream_port live_stream_name

import aiofiles
import aiohttp
import aiohttp.web
import asyncio
import io
import os
import pprint
import sys
import time

parent_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "../shared"))
if parent_dir not in sys.path:
  sys.path.append(parent_dir)

import command
import config

MOVIES_PATH = os.path.join(os.path.dirname(__file__), "../../movies")

if len(sys.argv) == 3:
  boat_name = sys.argv[1]
  LIVE_STREAM_NAME = sys.argv[2]
  config.load(boat_name)
  BOAT_STREAM_PORT = int(config.values["boat_stream_port"])
  CLIENT_STREAM_PORT = int(config.values["controller_stream_port"])
elif len(sys.argv) == 4:
  BOAT_STREAM_PORT = int(sys.argv[1])
  CLIENT_STREAM_PORT = int(sys.argv[2])
  LIVE_STREAM_NAME = sys.argv[3]
LIVE_STREAM_URL = (config.values["live_stream_url"][LIVE_STREAM_NAME]) if (LIVE_STREAM_NAME in config.values["live_stream_url"]) else None
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
  try:
    while True:
      data_queue = asyncio.Queue()
      client_queues.append(data_queue)
      size = await data_queue.get()
      await response.write(b'--' + my_boundary.encode("utf8") + b'\r\n')
      await response.write(b'Content-Type: image/jpeg\r\n')
      await response.write(b'X-Timestamp: ' + str(time.time()).encode("utf-8") + b'\r\n')
      await response.write(b'Content-Length: ' + str(size).encode("utf8") + b'\r\n\r\n')
      while True:
        data = await data_queue.get()
        if data == None:
          await response.write(b'\r\n')
          break
        await response.write(data)
    wc.shutdown()
  finally:
    pass
  return response

async def send_to_clients(current_client_queues, data):
  for client_queue in current_client_queues:
    await client_queue.put(data)

async def stream_handler(reader, writer):
  global client_queues
  addr = writer.get_extra_info('peername')
  print(f"stream {addr!r} is connected !!!!")
  current_client_queues = None
  try:
    while True:
      data = b''
      while len(data) < 6:
        result = await reader.read(6 - len(data))
        if result == b'':
          pprint.pprint("socket dead")
          break
        data += result
      if not data == b"frame,":
        pprint.pprint("no header: ")
        pprint.pprint(data)
        break;
      size_binary = await reader.read(4)
      expected_size = int.from_bytes(size_binary, "big")
      if expected_size == 0:
        continue
#      pprint.pprint("size :" + str(expected_size))
      current_client_queues = client_queues
      client_queues = []
      await send_to_clients(current_client_queues, expected_size)
      while expected_size > 0:
        data = await reader.read(expected_size)
        await send_to_clients(current_client_queues, data)
        expected_size -= len(data)
#        pprint.pprint("receive : " + str(len(data)) + " left: " + str(expected_size))
#      pprint.pprint("Send confirmation")
      writer.write(b'!')
      await send_to_clients(current_client_queues, None)
  finally:
    pprint.pprint("stream done")
    await send_to_clients(current_client_queues, None)
    writer.close()

async def write_images(path):
  global client_queues
  try:
    frame_counter = 0
    while True:
      filename = "frame_" + str(frame_counter).zfill(8) + ".jpg"
      filename_path = os.path.join(path, filename)
      data_queue = asyncio.Queue()
      client_queues.append(data_queue)
      size = await data_queue.get()
      async with aiofiles.open(filename_path, 'wb') as afp:
        while True:
          data = await data_queue.get()
          if data == None:
            break
          await afp.write(data)
      frame_counter += 1
    wc.shutdown()
  finally:
    pass

async def run_ffmpeg(client_stream_port, live_stream_url):
  if live_stream_url is None or live_stream_url == "":
    return
  while True:
    await asyncio.sleep(2)
    await command.run("/usr/bin/ffmpeg", "-threads", "4", "-vsync", "1", "-use_wallclock_as_timestamps", "1", "-i", "http://127.0.0.1:" + str(client_stream_port), "-vcodec", "libx264", "-b:v", "5M", "-f", "flv", live_stream_url)

def get_next_movie_path():
  global MOVIES_PATH
  dir_counter = 0
  while True:
    movie_dir = os.path.join(MOVIES_PATH, "movie_" + str(dir_counter))
    if not os.path.isdir(movie_dir):
      os.mkdir(movie_dir)
      pprint.pprint(movie_dir)
      break
    dir_counter += 1
  return movie_dir

def main(boat_stream_port, client_stream_port, live_stream_url):
  loop = asyncio.get_event_loop()

  loop.create_task(write_images(get_next_movie_path()))

  # Create server for to receive stream
  stream_server = asyncio.start_server(stream_handler, '0.0.0.0', boat_stream_port)
  stream_server = loop.run_until_complete(stream_server)
  print('Serving stream on {}'.format(stream_server.sockets[0].getsockname()))

  # Create http server for clients
  app = aiohttp.web.Application()
  app.router.add_get('/', client_handler)
  client_server = loop.create_server(app.make_handler(), '0.0.0.0', client_stream_port)
  client_server = loop.run_until_complete(client_server)
  print('Serving clients on {}'.format(client_server.sockets[0].getsockname()))

  loop.create_task(run_ffmpeg(client_stream_port, live_stream_url))

  loop.run_forever()

main(BOAT_STREAM_PORT, CLIENT_STREAM_PORT, LIVE_STREAM_URL)
