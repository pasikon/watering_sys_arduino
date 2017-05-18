-- local server
responseHeader = function(code, type) 
    return "HTTP/1.1 " .. code .. "\r\nConnection: close\r\nServer: nunu-Luaweb\r\nContent-Type: " .. type .. "\r\n\r\n";  
end

srv=net.createServer(net.TCP)  
srv:listen(80,function(connSv)

     connSv:on("receive",function(connSv,request)  

        -- pcall(googleTime())

        -- receive and parse WIFI config
        if wifi.ap.getip()~=nil then
		    pcall(parseRequest(request))
        end
		
		-- send http Header
        connSv:send(responseHeader("200 OK","text/html")); 

		-- Header
          connSv:send("<!DOCTYPE html>")
          connSv:send("<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"de\" lang=\"de\">\r\n")
          connSv:send("<head><title>ESP8266 Name " .. devName .. " Node " ..node.chipid().. "</title></head>\r\n")
          connSv:send("<body style=\"margin-left:20px; margin-top:10px;\">\r\n")

          -- Body
          connSv:send("<div>\r\n"); 
          -- connSv:send("Time: "..time.."\r\n");
          connSv:send("Device: " .. devName .. "\r\n");
          connSv:send("<br /> <br />\r\n");
          connSv:send("Node: " ..node.chipid().. "\r\n");
          connSv:send("<br /> <br />\r\n"); 
          connSv:send("Soil sensor 1: "..field1.." C\r\n");
          -- connSv:send("<br /> <br />\r\n");
          -- connSv:send("Temperature BMP: "..field6.." C\r\n");
          -- connSv:send("<br /> <br />\r\n");  
          -- connSv:send("Resistance Thermistor: "..field5.." \r\n");
          -- connSv:send("<br /> <br />\r\n");
          -- connSv:send("Temperature DHT11: "..field2.." C\r\n");


		-- Footer
          connSv:send("</div>\r\n</body>\r\n</html>"); 

          uart.write(0,request.."\r\n");
      
     end)

     connSv:on("sent",function(connSv)  
          connSv:close();  
          connSv = nil;     
     end)
     
end) 
