-- $Id: admin.lua 316 2010-01-14 22:48:51Z asus $
-- Config/Admun script
function puts(...)
   io.write(string.format(...))
end
function bind(host, port, backlog)
   puts(">socket.Lua: function bind =%s:%d\n", host, port)
   local sock, err = socket.tcp()
   if not sock then return nil, err end
   sock:setoption("reuseaddr", true)
   local res, err = sock:bind(host, port)
   if not res then return nil, err end
   res, err = sock:listen(backlog)
   if not res then return nil, err end
   return sock
end

print("\n>Adm-Lua: START Admin Script\n")
puts(">Adm-Lua: ENV LUA_PATH=>%s\n",os.getenv("LUA_PATH"))
puts(">Adm-Lua: ENV LUA_CPATH=>%s\n",os.getenv("LUA_CPATH"))

do
   local nth
   repeat
      nth = startWrkTh()
      puts(">Adm-Lua: start %d work thread\n",nth)
      ansleep(3)
   until nth == 8
end

-- Start signaling thread
do
   local m1 = 4
   puts(">Adm-Lua: start tstFsmTh thread %d\n",m1)
   tstFsmTh(m1)
   local m2, t1, t2 = 9, 9, 1
   puts(">Adm-Lua: start tstSgTh thread %d %d %d %d\n",m1,m2,t1,t2)
   tstSgTh(m1, m2, t1, t2)
end

-- Unlock ALL
puts("\n>Adm-Lua: START ALL \n\n")
aStart()
ansleep(77)

-- Start ADMIN loop
do
   local mCtrl
   local remLink, appExit = "", ""
   local Hst, Prt = "localhost", "9383"

   function tnLink(S)
      local er, ack, mCmd = "", "", ""
      local function runRCmd(C) ack = assert(loadstring(C))() end
      print(">Adm-Lua: srv NEW Connection\n")
      while (mCmd ~= "Quit") do
	 if (mCmd ~= "") then
	    er = pcall(runRCmd, mCmd)
	    if er then 
	       print(">Adm-Lua: Send ACK - ", ack)
	       S:send(tostring(ack) .. "\n")
	    end
	 end
	 mCmd = S:receive()
	 puts(">Adm-Lua: getRemCmd >%s<\n", mCmd)
	 if (mCmd == "appExit") then
	    print(">Adm-Lua: tnLink appExit\n")
	    S:send("Apps shutdown\n")
	    S:close()
	    appExit = "Exit"
	 end
      end
      S:send("Session closed, chao\n")
      S:close()
   end

   local mSrv = bind(Hst, Prt)
   print(">Adm-Lua: socket.binded\n")
   while (appExit == "") do
      print(">Adm-Lua: Srv waiting for connection...\n")
      mCtrl = mSrv:accept()
      print(">Adm-Lua: mSrv:accept\n")
      tnLink(mCtrl)
   end
   mSrv:close()
   puts(">Adm-Lua: exiting apps <%s>\n", appExit)
end

-- shutdown ITS
puts(">Adm-Lua: Stop new-Ctxt factory - %s\n", stopCFactory(1))
puts(">Adm-Lua: Stop sign.threads\n" , stopAllSgTh())
puts(">Adm-Lua: Close all active contexts\n", ctxtHashClean())
ansleep(3333)
puts(">Adm-Lua: Stop work-threads and timer\n", preExitApps())
ansleep(777)
print(">Adm-Lua: *** Exit admin script ***\n\n")
