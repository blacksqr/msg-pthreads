#!./tclSql
proc Echo_Client {host port} {
    set s [socket $host $port]
    fconfigure $s -buffering line
    return $s
}
set s [Echo_Client localhost 4567]

puts $s "wThrd start"
puts "Echo>> [gets $s]"
puts $s "wThrd start"
puts "Echo>> [gets $s]"

puts $s "apps xstat on"
puts "Echo>> [gets $s]"

# For shutdown
#puts $s "appExit"
