# $Id$
# Config/Admun script

# port - The server's port number
proc Echo_Server {port} {
    global apps_exit_var
    set s [socket -server EchoAccept -myaddr localhost $port]
    #vwait forever
    vwait apps_exit_var
}
#sock - The new socket connection to the client
#addr - The client's IP address
#port - The client's port number
proc EchoAccept {sock addr port} {
    global echo
    puts ">aTcl: Accept $sock from $addr port $port"
    set echo(addr,$sock) [list $addr $port]
    fconfigure $sock -buffering line
    # Set up a callback for when the client sends data
    fileevent $sock readable [list Echo $sock]
}
#sockThe socket connection to the client
proc Echo {sock} {
    global echo
    global apps_exit_var
    # Check end of file or abnormal connection drop, then echo data back to the client.
    if {[eof $sock] || [catch {gets $sock line}]} {
	close $sock
	puts ">aTcl: Close $echo(addr,$sock)"
	unset echo(addr,$sock)
    } else {
	puts ">>> $line"
	if { $line == "quit" } {
	    close $sock
	    puts ">aTcl: Client logout $echo(addr,$sock)"
	    unset echo(addr,$sock)
	} elseif { $line == "appExit" } {
	    puts ">aTcl: Admin application exit"
	    set apps_exit_var 1
	} else {
	    catch { eval $line } res
	    puts $sock $res
	}
    }
}
# uncomment - to block "exit" command in admin script
rename exit {}

#######################################

puts "\n\n>aTcl: *** START Admin Script ***\n"

puts ">aTcl: start work thread [wThrd start]"
puts ">aTcl: start work thread [wThrd start]"
puts ">aTcl: start work thread [wThrd start]"
puts ">aTcl: start work thread [wThrd start]"
puts ">aTcl: start work thread [wThrd start]"
puts ">aTcl: start work thread [wThrd start]"
# Work threads ready & waiting events

# set first hausekeeping timeout in 5.55 sec
puts ">aTcl: HsKeep tOut        [apps tout 555 2 0]"
# make NN msg/s
#puts ">aTcl: start xTst  thread [sThrd start xtst 1777]"
# make NN1 and NN2 msg/s TT1 and TT2 in 0.1 s
puts ">aTcl: start zTst  thread [sThrd start ztst 1111 1111  10 10]"
puts ">aTcl: start zTst  thread [sThrd start ztst 1111 11111 18 2]"
puts ">aTcl: start zTst  thread [sThrd start ztst 1111 11111 19 1]"
# Alarm generation thread NN1 NN@ msg/s and tmOut alarm
#puts ">aTcl: start aTst  thread [sThrd start atst 9 5]"
# UDP Thread - to read SIP msg
#puts ">aTcl: start UDP   thread [sThrd start udp 5066 121]"

# Start all
puts "\n>aTcl: START ALL[apps start]\n\n"
puts ">aTcl: Num of work threads running [wThrd num]"
####################################################
# Start ADMIN loop on localhost:4567
puts ">aTcl: Start Echo_Server [Echo_Server 4567]"
####################################################

puts ">aTcl: Stop Ctxt factory  [apps endNC 3]"
# wait 1,11 sec
after 1111
puts ">aTcl: Stop sign.threads  [apps goff]"
after 3333
puts ">aTcl: Stop work-threads  [apps woff]"
after 333
puts ">aTcl: Remove all context [apps halt]"
puts ">aTcl: *** Exit admin script ***"
# $Log$
