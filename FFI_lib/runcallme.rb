#!/opt/local/bin/ruby1.9

require 'ffi'

module CallMe
  extend FFI::Library
  ffi_lib "callme.bundle"
  callback :callme_callback, [:pointer], :void
  attach_function :watch_path, [:string, :callme_callback], :void

  class WatcherData < FFI::Struct
    layout :path, :string,
           :cb, :pointer,
           :secs, :int

    def to_s
      "#{self[:path]},#{self[:cb].inspect},#{self[:secs]}"
    end
  end
end

STDOUT.sync = true
Thread.abort_on_exception = true

Callback = Proc.new do |x|
  print "(Ruby:#{CallMe::WatcherData.new(x).to_s})"
end

puts "-- Passing Callback #{Callback.inspect}"

time0 = Time.now.to_i
Thread.new { while true do print "[#{Time.now.to_i - time0}]"; sleep 1; end }

CallMe.watch_path("somefile", Callback)

while Time.now.to_i < time0 + 30 do print '.'; sleep 0.5 end
puts "done"

