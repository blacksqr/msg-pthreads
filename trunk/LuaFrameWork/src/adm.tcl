#!./tclSql
proc Echo_Client {host port} {
    set s [socket $host $port]
    fconfigure $s -buffering line
    return $s
}
set s [Echo_Client localhost 4567]

puts $s [concat $argv]
puts "\n[concat $argv] >>> [gets $s]\n"

# $Id: adm.tcl 301 2010-01-11 12:08:00Z asus $
