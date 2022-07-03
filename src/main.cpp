#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>


const char* ssid = "iPhone";
const char* password = "tshetrim";

//Your Domain name with URL path or IP address with path
String serverName = "https://happy-fish.herokuapp.com/esp32/";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;

// Set LED GPIO
const int ledPin1 = 25;
const int ledPin2 = 26;
const int ledPin3 = 27;

//variables
String string = "";
String sliderValue1 = "0";
String sliderValue2 = "0";
String sliderValue3 = "0";

int dutyCycle1;
int dutyCycle2;
int dutyCycle3;

// setting PWM properties
const int freq = 5000;
const int ledChannel1 = 0;
const int ledChannel2 = 1;
const int ledChannel3 = 2;

const int resolution = 8;


//Json Variable to Hold Config Values
JSONVar config;


boolean getConfig(){
    HTTPClient http;

    String serverPath = serverName + "config";
    
    // Your Domain name with URL path or IP address with path
    http.begin(serverPath.c_str());
    
    // Send HTTP GET request
    int httpResponseCode = http.GET();
    
    if (httpResponseCode>0) {
      //prints the current HTTP response code 
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);

      //payload contains the JSON as a string with config variables
      String payload = http.getString();
      config = JSON.parse(payload);
      Serial.println(payload);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
}

void setup() {
  Serial.begin(115200); 

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}


void loop() {
  //Send an HTTP POST request every 10 minutes
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
        getConfig();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}