#!/bin/bash
# ./start.sh (boat|value_relay) <boat_name>

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
log_file="${DIR}/../daemon.log"
mv "${log_file}.1" "${log_file}.2"
mv "${log_file}" "${log_file}.1"
echo "$DIR" > "${log_file}"
while true
do
  echo "--" >> "${log_file}"
  date >> "${log_file}"
  if [ "$2" == "" ]; then
    "${DIR}/$1/server.py" >> "${log_file}" 2>&1
  else
    "${DIR}/$1/server.py" "$2" >> "${log_file}" 2>&1
  fi
done
