#!/bin/bash

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
log_file="${DIR}/../daemon.log"
mv "${log_file}.1" "${log_file}.2"
mv "${log_file}" "${log_file}.1"
echo "$DIR" > "${log_file}"
while true
do
  echo "--" >> "${log_file}"
  date >> "${log_file}"
  "${DIR}/$1" "$2" "${DIR}/../ulysse314.ini" >> "${log_file}" 2>&1
done
