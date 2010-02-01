-- LUA work thread script
--
function puts(...)
   io.write(string.format(...))
end

puts("- - - -- WrkTh LUA_PATH=>%s\n", os.getenv("LUA_PATH"))
puts("- - - -- WrkTh LUA_CPATH=>%s\n", os.getenv("LUA_CPATH"))

local wtobj, x
x = getWLuaObj()
wtobj, x = getWTObj(x)

local xt = 7 - wtobj:getThId()
--local led = assert(io.open(string.format("/sys/class/leds/led%d\:green/brightness",xt), "w+"))
--local led = io.open(string.format("/sys/class/leds/led%d\:green/brightness",xt), "w+")

do
   local z = 0
   puts("- - - -- WrkTh-%d wtobj=>%s<=\n", xt, wtobj:test())
   k, x = 0, 0
   local ev = 1
   while (ev ~= 0) do
      ev = wtobj:getEvent()
      if (x % 5) == 0 then
	 puts("- - - -- WrkTh-%d(%d) get LedEvent=%d %d\n", xt, z, ev, x%5)
	 --led:write(z)
	 z = 1 - z
      end
      puts("- - - -- WrkTh-%d(%d) get Event=%d %d\n", xt, z, ev, x%5)
      wnsleep(2)
      x = x + 1
   end
end

--led:close()
