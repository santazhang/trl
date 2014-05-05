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

# TODO real statistics
commit_count = 0
abort_count = 0
get '/stat' do
  content_type :json
  commit_count = (commit_count + 1) % 17
  abort_count = (abort_count + 1) % 16
  { :commit_count => commit_count, :abort_count => abort_count }.to_json
end

# root page
get '/' do
  redirect "/index.html"
end
