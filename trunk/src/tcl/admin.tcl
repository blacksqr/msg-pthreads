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
# Block "exit" command in admin script
rename exit {}

# Setup demo database & clean all log files
catch {
    file delete /tmp/its_demo.db
    file delete /tmp/its_demo.db-journal
}

sqlite3 db /tmp/its_demo.db
db eval {
    PRAGMA default_synchronous=OFF;
    PRAGMA default_temp_store=MEMORY;
    PRAGMA default_cache_size=16384;
    PRAGMA legacy_file_format = OFF;
    BEGIN; -- begin transaction

    CREATE TABLE _RemUser
    (
     uid   TEXT PRIMARY KEY,
     pwd   TEXT,    -- User password
     xTxt  TEXT,    -- User password
     pId   INTEGER, -- Provider ID
     IId   INTEGER, -- Provider ID
     memo  TEXT);
    CREATE INDEX indIid ON _RemUser(Iid);

    COMMIT; -- end transaction
}
puts ">>>> RemUser"
db close
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
# make NN1 and NN2 msg/s TT1 and TT2 in 0.1 s
puts ">aTcl: start xtst  thread [sThrd start xtst 777  999  10 10]"
puts ">aTcl: start xtst  thread [sThrd start xtst 555  3777 18 2]"
puts ">aTcl: start xtst  thread [sThrd start xtst 3777 555  1  19]"
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
puts ">aTcl: Stop sign.threads  [apps goff]"
puts ">aTcl: NOW Remove all context"
puts ">aTcl: Remove all context [apps halt]"
after 3333
puts ">aTcl: NOW Stop work-threads"
puts ">aTcl: Stop work-threads  [apps woff]"
puts ">aTcl: *** Exit admin script ***"
# $Log$
