#include <OneWire.h>
#include <SPI.h>
#include <Ethernet.h>

OneWire ds(7); 
byte mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0x28, 0xD8 };
IPAddress ip(192,168,11,101);
EthernetServer server(80);

void setup(void) 
{
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Ethernet.begin(mac, ip);
  server.begin();
}

void loop(void) 
{
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];
  int HighByte, LowByte, TReading, SignBit, Tc_100, Whole, Fract;


  EthernetClient client = server.available();
  if (client) {
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connnection: close");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
                    // add a meta refresh tag, so the browser pulls again every 5 seconds:
          client.println("<meta http-equiv=\"refresh\" content=\"5\">");
          client.print("Dallas OneWire: </br>");
      
  while ( ds.search(addr))
  {
    for( i = 0; i < 8; i++) //address output
    {
      client.print(addr[i], HEX);
      client.print(" ");
    }
    ds.write(0x44,1);    
    delay(1000);   
    present = ds.reset();
    ds.select(addr);    
    ds.write(0xBE); 
    for ( i = 0; i < 9; i++) 
    {     
      data[i] = ds.read();
    }
    LowByte = data[0];
    HighByte = data[1];
    TReading = (HighByte << 8) + LowByte;
    SignBit = TReading & 0x8000; 
    if (SignBit) 
    {
      TReading = (TReading ^ 0xffff) + 1; 
    }
    Tc_100 = (6 * TReading) + TReading / 4;    
    Whole = Tc_100 / 100;  
    Fract = Tc_100 % 100;
    if (SignBit) 
    {
       client.print("-");
    }
    client.print(Whole); //Temperature output
    client.print(".");
    if (Fract < 10)
    {
       client.print("0");
    }
    client.print(Fract);

    client.print("</br>");
  }
  client.println("No more address</br>");
  delay(10000); 
  client.println("</html>");
  break;
  }
  if (c == '\n') 
  {
     currentLineIsBlank = true;
  }
  else if (c != '\r') 
  {
     currentLineIsBlank = false;
  }
 }
}
delay(1);
client.stop();
Serial.println("client disonnected");
}
}
