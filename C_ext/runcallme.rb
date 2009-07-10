#!/opt/local/bin/ruby1.9

#require 'debug'
require 'callme'
include CallMeModule

class Object
  def to_b
    self.to_s.match(/^(|false|no|0)$/i) ? false : true
  end
end

STDOUT.sync = true
Thread.abort_on_exception = true

USE_C_THREADS = ENV['USE_C_THR'] ? ENV['USE_C_THR'].to_b : true
USE_RUBY_THREADS = ENV['USE_RUBY_THR'] ? ENV['USE_RUBY_THR'].to_b : false
USE_C_CALLBACK = ENV['USE_C_CB'] ? ENV['USE_C_CB'].to_b : false

callback = USE_C_CALLBACK ? nil : Proc.new { |x| print "(Ruby:#{x})" }

puts "-- Passing callback  #{callback.inspect}"
puts "-- Use Ruby threads? #{USE_RUBY_THREADS.to_s}"
puts "-- Use C threads?    #{USE_C_THREADS.to_s}"

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
