#!/usr/bin/env ruby

psaux = `ps aux | grep trl_client | grep -v grep`

psaux.each_line do |l|
  sp = l.split
  puts sp[1]
  Process.kill "KILL", sp[1].to_i
  break
end
