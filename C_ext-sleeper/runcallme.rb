#!/opt/local/bin/ruby1.9

require 'callme'
include CallMeModule

STDOUT.sync = true
Thread.abort_on_exception = true

time0 = Time.now.to_i
Thread.new { while true do print "[#{Time.now.to_i - time0}]"; sleep 1; end }

cb = Array.new
cb << Proc.new { |x| print "(cb0:#{x})" }
cb << Proc.new { |x| print "(cb1:#{x})" }

(1..12).each do |x|
  CallMe.start_stf(cb[x%2],x) if x%3 == 0
end

while Time.now.to_i < time0 + 20 do print '.'; sleep 0.5 end
puts "done"

