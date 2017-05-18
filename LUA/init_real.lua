-- basic config
wifiStation = "MOVISTAR_3C42"
wifiPW = "PNq6NJD3qeXbxHYMbC43"
devName = "ESP8266_podlewacz"
thingsSpeakKey = "1RUB0PWV56MZ3XRD"
tsChannel="270657"

-- Status LEDs
led0 = 3
gpio.mode(led0, gpio.OUTPUT); 
gpio.write(led0, gpio.LOW); 

led1 = 4
gpio.mode(led1, gpio.OUTPUT); 
gpio.write(led1, gpio.LOW); 

-- turn second LED on
gpio.write(led0, gpio.HIGH);

-- AP config
APWIFIcfg = 
{
     ssid="ESP" ..node.chipid(),
     pwd=node.chipid()
}

APIPcfg =
{
    ip="192.168.1.1",
    netmask="255.255.255.0",
    gateway="192.168.1.1"
}

-- ThingSpeak Values
field1=0;

-- Serial Setup
uart.setup(0,9600,8,0,0)

uart.write(0,"ESP8266 Hello!\r\n")

-- Start
print("Setting up WIFI...")
wifi.setmode(wifi.STATION)
wifi.sta.config(wifiStation,wifiPW)
wifi.sta.connect()
--uart.write(0,wifi.sta.getip())

i = 0
tmr.alarm(1, 1000, 1, function() 
     if wifi.sta.getip()==nil and wifi.ap.getip()==nil then 
          uart.write(0,"Try "..i..": IP unavaiable, Waiting...\r\n") 

          if i==iMax then
               -- tmr.stop(1)
               uart.write(0,"Start AP\r\n")
               wifi.setmode(wifi.SOFTAP)
               wifi.ap.setip(APIPcfg)
               wifi.ap.config(APWIFIcfg)
               -- gpio.write(led1, gpio.HIGH)
               -- dofile("localServer.lua")
               i = 0
          end
          
     else 
          tmr.stop(1)
          -- uart.write(0,"Config done.\r\n")
          
          gpio.write(led0, gpio.LOW);
                    
          if wifi.ap.getip()~=nil then
               -- dofile("run_config.lc")
               --dofile("localServer.lc")
               --dofile("wifiConfig.lc")
               
               lighton=0
               tmr.alarm(3,1000,1,function()
                    if lighton==0 then
                         gpio.write(led0, gpio.HIGH);
                         lighton=1
                    else
                         gpio.write(led0, gpio.LOW);
                         lighton=0
                    end
               end) 
          end
          
          if wifi.sta.getip()~=nil then
               gpio.write(led1, gpio.HIGH);
               --dofile("tempArduino.lc");
               --dofile("localServer.lc");
               --dofile("googleTime.lc");

               uart.write(0,"WS1\r\n")
          end
     end 
     
     i = i+1
end)

--tmr.alarm(1, 1000, 1, function() 
--    uart.write(0,":):):)\n")
--end)

function sendData()
    uart.write(0,"Sending data to thingspeak.com\r\n")
end
