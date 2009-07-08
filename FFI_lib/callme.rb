#!/opt/local/bin/ruby1.9

USE_RUBY_THREADS = true

require 'ffi'

module CallMe
  extend FFI::Library
  ffi_lib "libcallme.so"
  callback(:fp, [:int], :void)
  attach_function :watch_path, [:string, :fp, :int], :void
end

STDOUT.sync = true
Thread.abort_on_exception = true

cb_proc = Proc.new do |x|
  print "(ruby:#{x})"
end

Thread.new { while true do print '#'; sleep 1; end } ## heartbeat

puts "using ruby callback cb_proc"

if USE_RUBY_THREADS
  puts "using ruby threads"
  Thread.new { CallMe.watch_path("somefile",cb_proc,0) }
else
  puts "using C threads"
  CallMe.watch_path("somefile",cb_proc,1)
end

time_to_stop = Time.now.to_i + 30
while time_to_stop > Time.now.to_i do
  print '.'
  sleep 0.2
end

puts "done"

