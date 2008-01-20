#!../Tcl/unix/tclsh
proc Echo_Client {host port} {
    set s [socket $host $port]
    fconfigure $s -buffering line
    return $s
}
set s [Echo_Client localhost 4567]

set fc [open [lindex $argv 0] r]

while { [gets $fc line] >=0 } {
  #puts $line
  set list [split $line %]
  set pin "1[lindex $list 1]"
  set nr  [lindex $list 0]
  if {$pin != "1"} {
    puts $s "telCard import 500 0 0x0300 64 0 0 19 0 91 $pin $nr"
    puts "$pin $nr >>> [gets $s]"
  }
}
