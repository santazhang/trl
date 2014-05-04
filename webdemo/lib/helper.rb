def remote_run host, cmd
  cwd = Dir.pwd
  if ["127.0.0.1", "localhost"].include? host
    real_cmd = cmd
  else
    real_cmd = "ssh #{host} ; cd #{cwd} ; #{cmd}"
  end
  puts "cmd: #{real_cmd}"
  system real_cmd
end
