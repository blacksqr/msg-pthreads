#!./tclSql

# port - The server's port number
proc Echo_Server {port} {
    set s [socket -server EchoAccept -myaddr localhost $port]
    vwait forever
}
#sock - The new socket connection to the client
#addr - The client's IP address
#port - The client's port number
proc EchoAccept {sock addr port} {
    global echo
    # Record the client's information
    puts ">aTcl: Accept $sock from $addr port $port"
    set echo(addr,$sock) [list $addr $port]
    # Ensure that each "puts" by the server results in a network transmission
    fconfigure $sock -buffering line
    # Set up a callback for when the client sends data
    fileevent $sock readable [list Echo $sock]
}
#sockThe socket connection to the client
proc Echo {sock} {
    global echo
    # Check end of file or abnormal connection drop, then echo data back to the client.
    if {[eof $sock] || [catch {gets $sock line}]} {
	close $sock
	puts ">aTcl: Close $echo(addr,$sock)"
	unset echo(addr,$sock)
    } else {
	puts ">>> $line"
    }
}
Echo_Server 5777
