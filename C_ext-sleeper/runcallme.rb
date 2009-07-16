#!/opt/local/bin/ruby1.9

require 'callme'
include CallMeModule

STDOUT.sync = true
Thread.abort_on_exception = true

time0 = Time.now.to_i
Thread.new { while true do print "[#{Time.now.to_i - time0}]"; sleep 1; end }

cb_a = Proc.new { |x| print "(Ruby_A.#{x})" }
cb_b = Proc.new { |x| print "(Ruby_B)" }
cb_c = Proc.new { |x| print "(Ruby_C)" }

cb = [cb_a,cb_b,cb_c]

(1..12).each do |x|
  CallMe.start_stf(cb[x%3],x) if x%2 == 0
end

while Time.now.to_i < time0 + 14 do print '.'; sleep 0.5 end
puts "done"

