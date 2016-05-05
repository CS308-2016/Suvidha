
/*
 *
 * Team Id: <Team Id>
 * Author List: Deepak Verma, Bhupendra Singh Bhuarya, Tapish Raniwal, Naveen Sagar
 * Filename: Suvidha.ino
 * Functions: none
 * Global Variables: LI, FanSpeed, manual
 *
 */

#ifndef __CC3200R1M1RGC__
// Do not include SPI for CC3200 LaunchPad
#include <SPI.h>
#endif
#include <WiFi.h>
#include "DHT.h"
#include <aJSON.h>
#include "M2XStreamClient.h"
#define DHTPIN 28 // what pin we're connected to
#define DHTTYPE DHT11



int LI =0 ;
int FanSpeed =0;
int manual=0;
// your network name also called SSID
char ssid[] = "<Your network SSID>";
// your network password
char password[] = "<password>";
// your network key Index number (needed only for WEP)
int keyIndex = 0;

char deviceId[] = "<device ID (give by m2x website)>"; // Device you want to push to
char streamName[] = "<variable name>"; // Stream you want to push to
char m2xKey[] = "<M2X key>"; // Your M2X access key

WiFiClient client1;
M2XStreamClient m2xClient(&client1, m2xKey);



WiFiServer server(80);

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  
    pinMode(GREEN_LED, OUTPUT);
  
  Serial.begin(115200);      // initialize serial communication

  dht.begin();


  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to Network named: ");
  // print the network name (SSID);
  Serial.println(ssid); 
  // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED) {
    // print dots while we wait to connect
    Serial.print(".");
    delay(300);
  }
  
  Serial.println("\nYou're connected to the network");
  Serial.println("Waiting for an ip address");
  
  while (WiFi.localIP() == INADDR_NONE) {
    // print dots while we wait for an ip addresss
    Serial.print(".");
    delay(300);
  }

  Serial.println("\nIP Address obtained");
  
  // you're connected now, so print out the status  
  printWifiStatus();

  Serial.println("Starting webserver on port 80");
  server.begin();                           // start the web server on port 80
  Serial.println("Webserver started!");
}

int brightness=40;
int count=0;
int uploadCount=0;

void loop() {
  
  analogWrite(GREEN_LED, brightness*FanSpeed);
  
  if(count==10000)FanSpeed=0;
  
  
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  int lightIntensity = analogRead(A2);
  int proximity = digitalRead(A11);
  
  lightIntensity = 10  - lightIntensity/280 ; // Normalize
  int temp=3;
  int temp1=3;
  if(manual!=1)
    {
        if(lightIntensity>5)
      {LI=LI-2;
        
      }
      
        if(lightIntensity<5)
      {LI=LI+2;
       
      }
    }
  
  
  analogWrite(BLUE_LED , LI*brightness);
  
  int i = 0;
  int response = m2xClient.updateStreamValue(deviceId, "temp", temperature);
  int response1 = m2xClient.updateStreamValue(deviceId, "lightIntensity", lightIntensity);
  int response2 = m2xClient.updateStreamValue(deviceId, "humidity", humidity);
  uploadCount=0;
  
  
  Serial.print("M2X client response1 code: ");
  Serial.println(response);
    Serial.print("M2X client response2 code: ");
  Serial.println(response1);
    Serial.print("M2X client response3 code: ");
  Serial.println(response2);

      Serial.print("Proximity : ");
      Serial.println(proximity);
  if(proximity==0)count++;
  else count=0;
  
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    char buffer[150] = {0};                 // make a buffer to hold incoming data
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (strlen(buffer) == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.println("<html>");
            client.println("<h1> Hello , this is interface to control SUVIDHA </h1>");

client.print("Current Temperature : <textarea name=\"message\" rows=\"1\" cols=\"5\">");client.print(temperature);client.println("</textarea>");
client.println("<br>");
client.println("<br>");
client.println("<script type=\"text/javascript\">");
client.println("</script>");
client.println("Fan Speed Controller");
client.println("<br>");
client.println("<button onclick=\"location.href='/f0'\">0</button>");
client.println("<button onclick=\"location.href='/f1'\">1</button>");
client.println("<button onclick=\"location.href='/f2'\">2</button>");
client.println("<button onclick=\"location.href='/f3'\">3</button>");
client.println("<button onclick=\"location.href='/f4'\">4</button>");
client.println("<button onclick=\"location.href='/f5'\">5</button>");
client.println("<br>");
client.println("<br>");
client.println("<br>");
client.println("<br>");
client.print("Current Humidity: <textarea name=\"message\" rows=\"1\" cols=\"5\">");client.print(humidity);client.println("</textarea>");
client.println("<br>");
client.println("<br>");
client.print("Current Light Intensity: <textarea name=\"message\" rows=\"1\" cols=\"5\">");client.print(lightIntensity);client.println("</textarea>");
client.println("<br>");
client.println("<br>");
client.println("<script type=\"text/javascript\">");
client.println("</script>");
client.println("Light Intensity Controler");
client.println("<br>");
client.println("<button onclick=\"location.href='/l0'\">0</button>");
client.println("<button onclick=\"location.href='/l1'\">1</button>");
client.println("<button onclick=\"location.href='/l2'\">2</button>");
client.println("<button onclick=\"location.href='/l3'\">3</button>");
client.println("<button onclick=\"location.href='/l4'\">4</button>");
client.println("<button onclick=\"location.href='/l5'\">5</button>");

client.println("</html>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          }
          else {      // if you got a newline, then clear the buffer:
            memset(buffer, 0, 150);
            i = 0;
          }
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          buffer[i++] = c;      // add it to the end of the currentLine
        }

        // Checks the client's GET request
        if (endsWith(buffer, "GET /f0")) {
          FanSpeed=0;//digitalWrite(RED_LED, HIGH);               
          manual=1;
        }
        if (endsWith(buffer, "GET /f1")) {
          FanSpeed=1;//digitalWrite(RED_LED, HIGH);               
          manual=1;
        }
        if (endsWith(buffer, "GET /f2")) {
          FanSpeed=2;//digitalWrite(RED_LED, LOW);     
          manual=1;
        }
        
        if (endsWith(buffer, "GET /f3")) {
          FanSpeed=3;//digitalWrite(RED_LED, LOW);               
          manual=1;
        }
        
        if (endsWith(buffer, "GET /f4")) {
          FanSpeed=4;//digitalWrite(RED_LED, LOW);                
          manual=1;
        }
        
        if (endsWith(buffer, "GET /f5")) {
          FanSpeed=5;//digitalWrite(RED_LED, LOW);                
          manual=1;
        }
        if (endsWith(buffer, "GET /l0")) {
          LI=0;//digitalWrite(RED_LED, HIGH);              
          manual=1;
        }
        if (endsWith(buffer, "GET /l1")) {
          LI=1;//digitalWrite(RED_LED, HIGH);               
          manual=1;
        }
        if (endsWith(buffer, "GET /l2")) {
          LI=2;//digitalWrite(RED_LED, LOW);                
          manual=1;
        }
        
        if (endsWith(buffer, "GET /l3")) {
          LI=3;//digitalWrite(RED_LED, LOW);                
          manual=1;
        }
        
        if (endsWith(buffer, "GET /l4")) {
          LI=4;//digitalWrite(RED_LED, LOW);                
          manual=1;
        }
        
        if (endsWith(buffer, "GET /l5")) {
          LI=5;//digitalWrite(RED_LED, LOW);                
          manual=1;
        }
        
      }
    }
    // close the connection:
Serial.print("New FanSpeed: ");
Serial.println(FanSpeed);
    client.stop();
    Serial.println("client disonnected");
  }
  
  
  delay(500);
//  uploadCount++;

}

//
//a way to check if one array ends with another array
//
boolean endsWith(char* inString, char* compString) {
  int compLength = strlen(compString);
  int strLength = strlen(inString);
  
  //compare the last "compLength" values of the inString
  int i;
  for (i = 0; i < compLength; i++) {
    char a = inString[(strLength - 1) - i];
    char b = compString[(compLength - 1) - i];
    if (a != b) {
      return false;
    }
  }
  return true;
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}
