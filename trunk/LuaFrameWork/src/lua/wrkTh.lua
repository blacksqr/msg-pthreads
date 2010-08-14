-- LUA work thread script

debug = nil

function puts(...) io.write(string.format(...)) end

--

luaPath = "/Xxx/AsEee/LuaFr/src/lua/"
package.path = luaPath.."?;"..luaPath.."?.lua"
puts("- -- WrkTh package.path=>%s\n",  package.path)
require "sqlite3"

puts("- -- WrkTh LUA_PATH=>%s\n",  os.getenv("LUA_PATH"))
puts("- -- WrkTh LUA_CPATH=>%s\n", os.getenv("LUA_CPATH"))

local wtobj, x, xt
wtobj, x = getWTObj(getWLuaObj())

xt = wtobj:getThId()

--local led = assert(io.open(string.format("/sys/class/leds/led%d\:green/brightness",xt), "w+"))
--local led = io.open(string.format("/sys/class/leds/led%d\:green/brightness",xt), "w+")

-- Test Sqlite3
--local dbp = '/tmp/sql3Test'
--local db = assert( sqlite3.open(dbp) )

do
   local z = 0
   puts("- -- WrkTh-%d wtobj=>%s<=\n", xt, wtobj:test())
   k, x = 0, 0
   local ev = 1
   while (ev ~= 0) do
      ev = wtobj:getEvent()
      if (x % 5) == 0 then
	 puts("- -- WrkTh-%d(%d) get LedEvent=%d %d\n", xt, z, ev, x)
	 --led:write(z)
	 z = 1 - z
      end
      puts("- -- WrkTh-%d(%d) get Event=%d %d\n", xt, z, ev, x)
      wnsleep(3)
      x = x + 1

      --local stm = (x+3)..',2,"A new car","Cybercar XL-1000",65000.00,NULL,'..x..'.0'
      --db:exec('INSERT INTO invoice VALUES('..stm..');')
      --puts(">Adm-Lua: >%s< db:exec\n", stm)
   end
end

--led:close()

-- $Id: wrkTh.lua 375 2010-05-09 19:07:48Z asus $