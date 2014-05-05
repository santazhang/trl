#!/bin/bash

source conf/env.sh

for client_id in $(seq 1 $1 50); do
  RLOG_SERVER=noi.yzhang.net:8848 ../bin/trl_client $client_id &
done
