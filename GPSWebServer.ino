#include <TinyGPS++.h>
#include <ESP8266WiFi.h>

TinyGPSPlus gps;
static const uint32_t GPSBaud = 9600;

const char* ssid = "Titanic";
const char* password = "Automatic@100";

int ledPin = 2; // GPIO2
WiFiServer server(80);

String Latitude,Longitude;

unsigned long lastMillis = 0;    
byte c=0;

void setup() {
  Serial.begin(GPSBaud);            //Set the GPS baud rate.
  delay(10);
  
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
    
  WiFi.begin(ssid, password);
   
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
   
  // Start the server
  server.begin();
}
 
void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
   
  // Wait until the client sends some data
  while(!client.available()){
    delay(1);
  }

  if (millis() - lastMillis > 60*1000) 
  {
    // This sketch displays information every time a new sentence is correctly encoded.
   while (Serial.available() > 0)
  {
    if (gps.encode(Serial.read()))
    {
      getPositionInfo();
    }
  }
   lastMillis = millis(); // save the last time you published the data
  } 
    
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  client.flush();
   
  // Match the request
 
  int value = LOW;
  if (request.indexOf("/LED=ON") != -1) {
    digitalWrite(ledPin, LOW);
    value = LOW;
  } 
  
  if (request.indexOf("/LED=OFF") != -1){
    digitalWrite(ledPin, HIGH);
    value = HIGH;
  }
   
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");

  if (request.indexOf("/getGPS") != -1){    
  client.println("<body><h1>ESP8266 - GPS Interfacing</h1><h3>Latitude: ");
  client.println(Latitude);
  client.println("*</h3><h3> ");
  client.println("</h3><h3>Longitude: ");
  client.println(Longitude);
  client.println("%</h3><h3>");  
  client.println("</body>");  
  }
  
  client.print("Led pin is now: ");
  if(value == LOW) {
    client.print("On");  
  } else {
    client.print("Off");
  }
  client.println("<br><br>");
  client.println("Click <a href=\"/getGPS\">here</a> Get the GPS Position <br>");
  client.println("Click <a href=\"/LED=ON\">here</a> turn the LED on pin 2 ON<br>");
  client.println("Click <a href=\"/LED=OFF\">here</a> turn the LED on pin 2 OFF<br>");
  client.println("</html>");
 
  delay(1);
}

void getPositionInfo()
{
  if (gps.location.isValid())
  {
    Latitude = String(gps.location.lat(), 6);
    Longitude = String(gps.location.lng(), 6);
  }
  else
  {
    Latitude = "Error finding";
    Longitude = "Position"; 
  }
}
 

