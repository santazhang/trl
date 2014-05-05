#!/usr/bin/env ruby

IO.popen("QPS_INTERVAL=0.1 ../bin/rlogserver").each_line do |line|
  if line =~ /\| qps 'commit_count'/
    commit_count = line.split[7][2..-1].to_i
    File.open("log/commit.count~", "w") do |f|
      f.write "#{Time.now.to_f} #{commit_count}"
    end
    File.rename("log/commit.count~", "log/commit.count")
  elsif line =~ /\| qps 'abort_count'/
    abort_count = line.split[7][2..-1].to_i
    File.open("log/abort.count~", "w") do |f|
      f.write "#{Time.now.to_f} #{abort_count}"
    end
    File.rename("log/abort.count~", "log/abort.count")
  end
end
