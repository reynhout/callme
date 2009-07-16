#!/opt/local/bin/ruby1.9

require 'callme'
include CallMeModule

STDOUT.sync = true
Thread.abort_on_exception = true

time0 = Time.now.to_i
Thread.new { while true do print "[#{Time.now.to_i - time0}]"; sleep 1; end }

cb_a = Proc.new { |x| print "(Ruby_A.#{x})" }
cb_b = Proc.new { |x| print "(Ruby_B)" }

CallMe.start_stf(cb_a,4)
CallMe.start_stf(cb_a,2)
CallMe.start_stf(cb_a,5)
CallMe.start_stf(cb_b,7)
CallMe.start_stf(cb_b,9)

while Time.now.to_i < time0 + 10 do print '.'; sleep 0.5 end
puts "done"

