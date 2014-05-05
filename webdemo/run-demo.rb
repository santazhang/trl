#!/usr/bin/env ruby

require "#{File.dirname __FILE__}/lib/helper"

servers = File.read("#{File.dirname __FILE__}/conf/servers.txt").each_line.select {|l| l.strip != "" and not l.start_with? "#"}

begin
  servers.each_with_index do |addr, i|
    host, port = addr.split ":"
    remote_run host, "cd #{File.dirname __FILE__}/.. ; ./bin/trl_server #{i} &"
  end
  system "cd '#{File.dirname __FILE__}' ;ruby application.rb"
ensure
  servers.each do |addr|
    host, port = addr.split ":"
    remote_run host, "pkill -9 trl_server ; pkill -9 trl_client"
  end
end
