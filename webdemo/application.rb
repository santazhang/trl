require 'rubygems'
require 'bundler/setup'
require 'sinatra'
require 'json'
require File.join(File.dirname(__FILE__), 'environment')

error do
  e = request.env['sinatra.error']
  Kernel.puts e.backtrace.join("\n")
  'Application error'
end

# helpers do
#   # add your helpers here
# end

get '/update' do
  name = params["name"]
  value = params["value"]
  File.open("conf/config.ini") do |f|
    File.open("conf/config.ini~", "w") do |g|

      f.each_line do |line|
        line = line.strip
        if line.start_with? "#" or line == ""
          g.write line + "\n"
        else
          sp = line.split(":")
          key = sp[0].strip
          if key == name
            g.write "#{key}: #{value}\n"
          else
            g.write line + "\n"
          end
        end
      end

    end
  end
  File.rename("conf/config.ini~", "conf/config.ini")
  content_type :json
  { :name => name, :value => value }.to_json
end

get '/config' do
  content_type :json
  conf = {}
  File.open("conf/config.ini") do |f|
    f.each_line do |line|
      line = line.strip
      if line.start_with? "#" or line == ""
        next
      end
      sp = line.split(":")
      key = sp[0].strip
      value = sp[1].strip.to_i
      conf[key] = value
    end
  end
  conf.to_json
end

get '/stat' do
  content_type :json
  commit_count = 0
  abort_count = 0

  if File.exists? "log/commit.count"
    File.open("log/commit.count", "r") do |f|
      sp = f.read.split
      ts, commit_count = sp[0].to_f, sp[1].to_i
      if Time.now.to_f - ts > 1
        commit_count = 0
      end
    end
  end

  if File.exists? "log/abort.count"
    File.open("log/abort.count", "r") do |f|
      sp = f.read.split
      ts, abort_count = sp[0].to_f, sp[1].to_i
      if Time.now.to_f - ts > 1
        abort_count = 0
      end
    end
  end

  { :commit_count => commit_count, :abort_count => abort_count }.to_json
end

# root page
get '/' do
  redirect "/index.html"
end
