#!/bin/bash

source conf/env.sh

for client_id in $(seq $1 $2 $3); do
  RLOG_SERVER=noi.yzhang.net:8848 ../bin/trl_client $client_id &
done
