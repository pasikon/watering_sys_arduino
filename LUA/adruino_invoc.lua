function sendTest() 
    --uart.write(0,"ESP8266 reply :)\r\n")
    print("ESP8266 reply :)\r\n")
end

function getIp() 
    if wifi.sta.getip()==nil then
        print("No IP :(")
    else
        print(wifi.sta.getip())
    end
end
