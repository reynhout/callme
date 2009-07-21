#!/opt/local/bin/ruby1.9

require 'ffi'

module CallMe
  extend FFI::Library
  ffi_lib "callme.bundle"
  callback :callme_callback, [:int], :void
  attach_function :watch_path, [:string, :callme_callback], :void
end

STDOUT.sync = true
Thread.abort_on_exception = true

Callback = Proc.new { |x| print "(Ruby:#{x})" }
puts "-- Passing Callback #{Callback.inspect}"

time0 = Time.now.to_i
Thread.new { while true do print "[#{Time.now.to_i - time0}]"; sleep 1; end }

CallMe.watch_path("somefile", Callback)

while Time.now.to_i < time0 + 30 do print '.'; sleep 0.5 end
puts "done"

