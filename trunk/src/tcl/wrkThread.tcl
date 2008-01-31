# to send statistic info
proc Echo_Client {host port} {
    set s [socket $host $port]
    fconfigure $s -buffering line
    return $s
}
#if {[catch {set sOut [Echo_Client 192.168.1.3 45777]} c_res]} {
if {[catch {set sOut [Echo_Client localhost 5777]} c_res]} {
    puts "Err: $c_res ==> icsEvent set sOut = /dev/null"
    set sOut [open "/dev/null" w]
}

# $wThId - value from C++ wrkThread-Id
puts ">TID<$wThId>  *** Work thread run ***"
set nTclCall 0

puts "icsEvent open database [sqlite3 db /tmp/its_demo.db]"
# Max lock wait to 0,33 sec
#puts "Lock wait time-out to [db timeout 333]"
puts "Set synchronous=OFF     >[db eval {PRAGMA synchronous  = OFF;}]"
puts "Set locking_mode=NORMAL >[db eval {PRAGMA locking_mode = NORMAL;}]"
puts "Set Read uncommitted >[db eval {PRAGMA read_uncommitted = 1;}]"
puts "Set Page size >[db eval {PRAGMA page_size = 16384;}]"

#######################################

#puts ">aTcl: Tcl DB commands"
#load libdb_tcl-4.6.so
#puts ">aTcl: berkdb create env"
#set e [berkdb env -create -home /tmp/Db -thread -cdb]
#puts ">aTcl: berkdb open in Env"
#set db [berkdb open -env $e -thread -create -hash tables.db test]
#set db1 [berkdb open -env $e -thread -create -btree tables1.db test1]

#######################################

proc on_busy {cnt} {
    global wThId
    puts $sOut ">TID<$wThId> on_busy $cnt"
    puts       ">TID<$wThId> on_busy $cnt"
    after 7
    return 0
}
puts "Set onBusy hook [db busy on_busy]"

# A procedure to execute SQL
proc execsql {sql {db db}} {
  # puts "SQL = $sql"
  uplevel [list $db eval $sql]
}
# Execute SQL and catch exceptions.
proc catchsql {sql {db db}} {
  # puts "SQL = $sql"
  return [lappend [catch {$db eval $sql} m] $m]
}

proc tstCtxt {ev} {
    global wThId sOut valTstLst nTclCall
    while {$ev} {
	set tbInd 0
	set NnRec 0
	set valTstLst [aCtxt tstData]
	set insVal "\'[join $valTstLst "\',\'"]\'"
	#puts ">TID<$wThId> db eval INSERT INTO _RemUser VALUES($insVal);"
	set ctRes [catch {
	    execsql "INSERT INTO _RemUser VALUES($insVal);" db
	    set tbInd [db last_insert_rowid]
	} cRes]
	puts $sOut ">TID<$wThId> db INS Result >> $ctRes> $cRes"
	incr ev -1
    }
}

while 1 {
    #set QQ [clock format [clock seconds] -format {%d-%k:%M:%S}]
    # [getev] - set value of $ctxtType
    set Ev  [getev]
    incr nTclCall
    puts ">TID<$wThId> Context_Type-$ctxtType Event-$Ev"
    if { !$Ev } {
	# Reload work script
	puts ">TID<$wThId> EXIT wrkTclScript [db close]"
	break
    }
    switch -- $ctxtType {
	1 {
	    # HausKeep Context
	    puts $sOut ">TID<$wThId> HausKeep CtxtType-$ctxtType Ev-$Ev"
	} 2 {
	    # Test Context
	    after 1
	    if {$wThId == 3} {
		puts $sOut ">TID<$wThId> Test Context type $ctxtType Ev $Ev"
	    }
	} 3 {
	    # DB Context
	    puts ">TID<$wThId> TRANS-Result tstCtxt $Ev>[db transaction [list tstCtxt $Ev]]<"
	    puts $sOut ">TID<$wThId> $nTclCall ===>SQL"
	    puts       ">TID<$wThId> $nTclCall ===>SQL"
	} default {
	    # Unknown Context - error
	    puts "Unknown Context - error $ctxtType"
	}
    }
}
