-- Config/Admun script

debug = nil

function puts(...) io.write(string.format(...)) end

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

puts(">Adm-Lua: *** START Admin Script ***\n")

luaPath = "/Xxx/AsEee/LuaFr/src/lua/"
package.path = luaPath.."?;"..luaPath.."?.lua"
puts("- -- Admin package.path=>%s\n",  package.path)
puts(">Adm-Lua: ENV LUA_PATH=>%s\n", os.getenv("LUA_PATH"))
puts(">Adm-Lua: ENV LUA_CPATH=>%s\n",os.getenv("LUA_CPATH"))

--------------------------------------------------------
--------------------------------------------------------
--------------------------------------------------------
-- Test Sqlite3

local dbp = '/tmp/sql3Test'

os.remove(dbp)
local db = assert( sqlite3.open(dbp) )
--local db = assert( sqlite3:open_memory() )
puts("- -- sqlite3:open=>%s\n",  dbp)

assert( db:exec[[
  CREATE TABLE customer (
    id		INTEGER PRIMARY KEY, 
    name	VARCHAR(40)
  );

  CREATE TABLE invoice (
    id		INTEGER PRIMARY KEY,
    customer	INTEGER NOT NULL,
    title	VARCHAR(80) NOT NULL,
    article1	VARCHAR(40) NOT NULL,
    price1	REAL NOT NULL,
    article2	VARCHAR(40),
    price2	REAL
  );

  CREATE TABLE invoice_overflow (
    id		INTEGER PRIMARY KEY,
    invoice	INTEGER NOT NULL,
    article	VARCHAR(40) NOT NULL,
    price	REAL NOT NULL
  );

  INSERT INTO customer VALUES( 1, "Michael" );
  INSERT INTO invoice VALUES( 1, 1, "Computer parts", "harddisc", 89.90, "floppy", 9.99 );
  INSERT INTO customer VALUES( 2, "John" );
  INSERT INTO invoice VALUES( 2, 2, "Somme food", "apples", 2.79, "pears", 5.99 );
  INSERT INTO invoice_overflow VALUES( NULL, 2, "grapes", 6.34 );
  INSERT INTO invoice_overflow VALUES( NULL, 2, "strawberries", 4.12 );
  INSERT INTO invoice_overflow VALUES( NULL, 2, "tomatoes", 6.17 );
  INSERT INTO invoice VALUES( 3, 2, "A new car", "Cybercar XL-1000", 65000.00, NULL, NULL );
]] )

local function customer_name(id)
  local stmt = db:prepare("SELECT name FROM customer WHERE id = ?")
  stmt:bind_values(id)
  stmt:step()
  local r = stmt:get_uvalues()
  stmt:finalize()
  return r
end

local function all_invoices()
  return db:nrows("SELECT id, customer, title FROM invoice")
end

local function all_articles(invoice)
   local function iterator()
      local stmt, row

      -- Get the articles that are contained in the invoice table itself.
      stmt = db:prepare("SELECT article1, price1, article2, price2 FROM invoice WHERE id = ?")
      stmt:bind_values(invoice)
      stmt:step()
      row = stmt:get_named_values()

      -- Every Invoice has at least one article.
      coroutine.yield(row.article1, row.price1)

      -- Maybe the Invoice has a second article?
      if row.article2 then

	 -- Yes, there is a second article, so return it.
	 coroutine.yield(row.article2, row.price2)

	 -- When there was an second article, maybe there are even
	 -- more articles in the overflow table? We will see...
	 stmt = db:prepare("SELECT article, price FROM invoice_overflow WHERE invoice = ? ORDER BY id")
	 stmt:bind_values(invoice)
	 
	 for row in stmt:nrows() do
	    coroutine.yield(row.article, row.price)
	 end
      end
   end
   return coroutine.wrap(iterator)
end

for invoice in all_invoices() do
   local id    = invoice.id
   local name  = customer_name(invoice.customer)
   local title = invoice.title

   print()
   print("Invoice #"..id..", "..name..": '"..title.."'")
   print("----------------------------------------")

   for article, price in all_articles(id) do
      print( string.format("%20s  %8.2f", article, price) )
   end
   print()
end

local xz  = 33
do
   db:exec('begin;')
   repeat
      xz = xz + 1
      local key = 3 + xz
      local stm = key..',2,"A new car","Cybercar XL-1000",65000.00,NULL,'..xz..'.0'
      db:exec('INSERT INTO invoice VALUES('..stm..');')
      puts(">Adm-Lua: >%s< db:exec\n", stm)
   until xz == 221
   db:exec('commit;')
end

db:close()
--------------------------------------------------------
--------------------------------------------------------
--------------------------------------------------------

do
   local nth
   repeat
      nth = startWrkTh()
      puts(">Adm-Lua: start %d work thread\n",nth)
      ansleep(3)
   until nth == 4 -- Start # work-threads pool
end

-- Start signaling thread
do
   local m1 = 29
   puts(">Adm-Lua: start tstFsmTh thread %d\n",m1)
   tstFsmTh(m1)
   local m2, t1, t2 = 333, 18, 2
   puts(">Adm-Lua: start tstSgTh thread %d %d %d %d\n",m1,m2,t1,t2)
   tstSgTh(m1, m2, t1, t2)
   m2 = 117
   puts(">Adm-Lua: start tstSgTh thread %d %d\n",m1,m2)
   tstSgThX(m1, m2)
end

-- Unlock ALL
puts("\n>Adm-Lua: START ALL \n\n")
aStart()
ansleep(77)

-- ADMIN loop
do
   local mCtrl
   local remLink, appExit = "", ""
   local Hst, Prt = "localhost", "4567"

   function tnLink(S)
      local er, ack, mCmd = "", "", ""
      local function runRCmd(C) ack = assert(loadstring(C))() end
      puts(">Adm-Lua: srv NEW Connection\n")
      while (mCmd ~= "Quit") do
	 if (mCmd ~= "") then
	    er = pcall(runRCmd, mCmd)
	    if er then 
	       puts(">Adm-Lua: Send ACK - ", ack)
	       S:send(tostring(ack) .. "\n")
	    end
	 end
	 mCmd = S:receive()
	 puts(">Adm-Lua: getRemCmd >%s<\n", mCmd)
	 if (mCmd == "appExit") then
	    puts(">Adm-Lua: tnLink appExit\n")
	    S:send("Apps shutdown\n")
	    S:close()
	    appExit = "Exit"
	 end
      end
      S:send("Session closed, chao\n")
      S:close()
   end

   local mSrv = bind(Hst, Prt)
   puts(">Adm-Lua: socket.binded\n")
   while (appExit == "") do
      puts(">Adm-Lua: Srv waiting for connection...\n")
      mCtrl = mSrv:accept()
      puts(">Adm-Lua: mSrv:accept\n")
      tnLink(mCtrl)
   end
   mSrv:close()
   puts(">Adm-Lua: exiting apps <%s>\n", appExit)
end

-- shutdown ITS
puts(">Adm-Lua: Stop new-Ctxt factory - %s\n",  stopCFactory(1))
puts(">Adm-Lua: Stop sign.threads\n",           stopAllSgTh())
puts(">Adm-Lua: Close all active contexts\n",   ctxtHashClean())
ansleep(3333)
puts(">Adm-Lua: Stop work-threads and timer\n", preExitApps())
ansleep(777)
puts(">Adm-Lua: *** Exit admin script ***\n\n")

-- $Id: admin.lua 387 2010-05-15 21:02:11Z asus $
