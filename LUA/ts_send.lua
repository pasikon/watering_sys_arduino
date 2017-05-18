-- ThingSpeak Values
field1=0;

function sendThingspeak()
     -- conection to thingspeak.com
     --uart.write(0,"Sending data to thingspeak.com\r\n")

     connD=net.createConnection(net.TCP, 0) 
     
     connD:on("receive", function(connD, payload) print(payload) end)
          
     connD:on("sent",function(connD)
          uart.write(0,"Closing connection\r\n")
          connD:close()
          -- Send Sent-Status to Arduino
          uart.write(0,"sent\r\n")
          connD=nil
     end)
     
     connD:on("disconnection", function(connD)
          -- uart.write(0,"Got disconnection...\r\n")
          uart.write(0,"D1\r\n")
          -- connD:close()
     end)

     -- api.thingspeak.com 184.106.153.149
     connD:connect(80,'184.106.153.149') 
     
     connD:send("GET /update?key="..thingsSpeakKey.."&field1="..field1.." HTTP/1.1\r\n") 
     connD:send("Host: api.thingspeak.com\r\n")
     connD:send("Accept: */*\r\n")
     connD:send("User-Agent: Mozilla/4.0 (compatible; esp8266 Lua; Windows NT 5.1)\r\n")
     connD:send("\r\n")
     
end
