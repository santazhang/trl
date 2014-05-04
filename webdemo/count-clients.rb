#!/usr/bin/env ruby

psaux = `ps aux | grep trl_client | grep -v grep`
puts psaux.each_line.count
