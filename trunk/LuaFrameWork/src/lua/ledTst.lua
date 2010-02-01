--
local xled = {}
xled[1] = assert(io.open("/sys/class/leds/led1\:green/brightness", "w+"))
xled[2] = assert(io.open("/sys/class/leds/led2\:green/brightness", "w+"))
-- .....
xled[8] = assert(io.open("/sys/class/leds/led8\:green/brightness", "w+"))

--xled[1] = assert(io.popen("/usr/bin/tee >> /sys/class/leds/led1\\:green/brightness", "w"))
--xled[2] = assert(io.popen("/usr/bin/tee >> /sys/class/leds/led2\\:green/brightness", "w"))
-- .....
--xled[8] = assert(io.popen("/usr/bin/tee >> /sys/class/leds/led8\\:green/brightness", "w"))

xled[1]:write(1)
xled[2]:write(1)
--xled[8]:write(1)

local y
local led

for xt=1,10000 do

print("Make LED On")
for xt=2,7 do
   led = io.open(string.format("/sys/class/leds/led%d\:green/brightness",xt), "w+")
   led:write(1)
   led:close()
end
for xx=1,100000 do
   y = (6546.0 + xx)/5245.0
end
print("Make LED Off")

for xt=2,7 do
   led = io.open(string.format("/sys/class/leds/led%d\:green/brightness",xt), "w+")
   led:write(0)
   led:close()
end

for xx=1,100000 do
   y = (6546.0 + xx)/5245.0
end
end
