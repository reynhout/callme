#!/opt/local/bin/ruby1.9

#require 'debug'
require 'callme'
include CallMeModule

USE_RUBY_THREADS = false #|| true
USE_C_THREADS = false || true
USE_RUBY_CALLBACK = false || true

STDOUT.sync = true
Thread.abort_on_exception = true

callback = USE_RUBY_CALLBACK ? Proc.new { |x| print "(Ruby:#{x})" } : nil

puts "-- Passing callback address (#{callback.inspect})"
puts "-- Use Ruby threads? #{USE_RUBY_THREADS.to_s}"
puts "-- Use C threads? #{USE_C_THREADS.to_s}"

Thread.new { while true do print '#'; sleep 1; end } ## heartbeat

if USE_RUBY_THREADS
  Thread.new { CallMe.watch_path("somefile", callback, USE_C_THREADS) }
else
  CallMe.watch_path("somefile", callback, USE_C_THREADS)
end

time_to_stop = Time.now.to_i + 30
while time_to_stop > Time.now.to_i do
  print '.'
  sleep 0.2
end

puts "done"

