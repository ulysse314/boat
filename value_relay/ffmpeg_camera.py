#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import signal
import subprocess
import threading

parent_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "../shared"))
if parent_dir not in sys.path:
  sys.path.append(parent_dir)

ffserver_proccess = None
ffserver_thread = None
running = False
process_lock = threading.Lock()
  
def start():
  global process_lock
  global running
  if running:
    return
  process_lock.acquire()
  running = True
  start_process()
  process_lock.release()

def stop():
  global ffserver_proccess
  global process_lock
  global running
  if not running:
    return
  process_lock.acquire()
  running = False
  if ffserver_proccess:
    os.kill(ffserver_proccess.pid, signal.SIGKILL)
    ffserver_proccess = None
  process_lock.release()

def start_process():
  global ffserver_proccess
  global ffserver_thread
  ffserver_proccess = subprocess.Popen(["/usr/bin/ffserver", "-f", "/home/ulysse314/ffserver.conf"])
  ffserver_thread = threading.Thread(target=wait_end_process)
  ffserver_thread.start()

def wait_end_process():
  global ffserver_proccess
  global ffserver_thread
  global running
  global process_lock
  ffserver_proccess.wait()
  process_lock.acquire()
  if not running:
    ffserver_thread = None
  else:
    start_process()
  process_lock.release()
