### The main goals of presented framework: ###
  * optimized Performance/Load ratio on SMP hardware

  * any threads architecture and application communication muster can be implemented

  * hiding thread synchronization and data locking issues from framework user

  * framework supports self diagnostic and alarm generation on fail

  * TCL (or LUA) as an administration, configuration and application extension language

  * very compact, applicable for embedded use

Not documented yet, but includes working example with database interface (to SQLite).

To compile from source you'll need tcl8.4.17(last stable) and sqlite-3.5.4.

**to start DEMO:**

> _**cd msg-pthreads**_

> _**.`  `./set\_env.sh**_

> _**cd src**_

> _**./its -D -d 4 -f /tmp/its.log**_

| **-D**      | run as a daemon                                      |
|:------------|:-----------------------------------------------------|
| **-d #**    | set debug level (from 0 to 4), 4 produce max. output |
| **-f <..>** | name of the log-file                                     |

**to administrate DEMO:**

> _**./tn.sh**_

| **wThrd start** | start new work thread                                 |
|:----------------|:------------------------------------------------------|
| **wThrd off #** | stop # work threads, don't stop all running :-)       |
| **wThrd num**   | return the number of running work threads             |
| **.....**       | see in script samples - tcl/admin.tcl; client.tcl ... |
| **appExit**     | to exit DEMO                                          |

The **Srv.tcl** is an emulation of administration console, it can be started
in a different terminal on the same host, or (if You edit _wrkThread.tcl_) on
a different host (it use telnet protocol). It show an output from **wrkThread.tcl**
script. To collect data it must be started before the DEMO.