#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>


const char* ssid = "SSID"; //input ssid here
const char* password = "Password"; //input password to network here 

//Your Domain name with URL path or IP address with path
String serverName = "https://happy-fish.herokuapp.com/esp32/";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 2000;

// Set LED GPIO
const int redPin = 25;
const int greenPin = 26;
const int bluePin = 27;

int rValue = 0;
int gValue = 0;
int bValue = 0;

int tod; // "20:19:29"
int sunrise_start; // "08:00"
int sunset_start; // "20:00"
int duration; // 30

enum stage {pre_sunrise, sunrise, lights_on, sunset, post_sunset};

int timeCheck = 0;

stage getStage(int seconds, int duration) {
  if (seconds < sunrise_start) {
    return pre_sunrise;
  } 
  if (seconds < sunrise_start + duration) {
    return sunrise;
  }
  if (seconds < sunset_start) {
    return lights_on;
  }
  if (seconds < sunset_start + duration) {
    return sunset;
  }
  else {
    return post_sunset;
  }
}

float getBrightness(int seconds, int duration) {

  stage s = getStage(seconds, duration);
  Serial.print("stage: ");
  Serial.println(s);

  if (s == pre_sunrise || s == post_sunset) {
    return 0.0;
  }

  if (s == sunrise) {
    return float(seconds - sunrise_start) / float(duration);
  }

  if (s == sunset) {
    return 1 - (float(seconds - sunset_start) / float(duration));
  }

  return 1.0;
}


void writeLED(int r, int g, int b){
  analogWrite(redPin, r);
  analogWrite(bluePin, b);
  analogWrite(greenPin, g);
}


//time format: hh:mm:ss
int timeInSeconds(String time){
  int firstColon = time.indexOf(':');
  int hour = time.substring(0,firstColon).toInt();
  int minutes =  time.substring(firstColon+1,firstColon+3).toInt();
  int seconds = time.substring(firstColon+3).toInt();
  return hour*60*60+minutes*60+seconds;
}

void assignConfigVariables(JsonObject doc){
    String todStr = doc["tod"]; // "20:29:39"
    String sunriseStr = doc["sunrise"]; // "08:00"
    String sunsetStr = doc["sunset"]; // "20:00"
    tod = timeInSeconds(todStr);

    //testing time skip 
    //tod = 25200 + timeCheck;
    //tod = 71998 + timeCheck;
    //timeCheck = timeCheck + 1800; //increment by 1 minute each time 

    sunrise_start = timeInSeconds(sunriseStr);
    sunset_start = timeInSeconds(sunsetStr);
    duration = doc["duration"]; // 30
      duration = duration * 60;
    rValue = doc["rValue"]; // 0
    gValue = doc["gValue"]; // 255
    bValue = doc["bValue"]; // 255

    float brightness = getBrightness(tod, duration);
    Serial.print("Brightness: ");
    Serial.println(brightness);
    rValue = (int)(rValue*brightness); // 0
    gValue = (int)(gValue*brightness); // 255
    bValue = (int)(bValue*brightness); // 255
    
    Serial.print("r: ");
    Serial.println(rValue);
    Serial.print("g: ");
    Serial.println(gValue);
    Serial.print("b: ");
    Serial.println(bValue);

    Serial.print("tod in time: ");
    Serial.println(tod);
        Serial.print("sunrise in time: ");
    Serial.println(sunrise_start);
        Serial.print("sunset in time: ");
    Serial.println(sunset_start);
        Serial.print("duration: ");
    Serial.println(duration);
}

void connectToWifi(){
  WiFi.begin(ssid, password);
  Serial.println("Connecting to wifi");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}


void setup() {
  Serial.begin(115200); 

  connectToWifi();
 
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
}


void loop() {
  //Send an HTTP POST request every timerDelay minutes
  if ((millis() - lastTime) > timerDelay) {
    Serial.println("Timer delay has passed, checking wifi status");
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      Serial.println("Wifi is connected, trying to send get request");
      HTTPClient http;

      String serverPath = serverName + "config";

      // Your Domain name with URL path or IP address with path
      http.begin(serverPath.c_str());
      
      // Send HTTP GET request
      int httpResponseCode = http.GET();
      if (httpResponseCode>0) {
        //getting response code
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);

        //payload JSON as string 
        String input = http.getString();
        Serial.println(input);


        StaticJsonDocument<384> doc;
        DeserializationError error = deserializeJson(doc, input);

        if (error) {
          Serial.print("deserializeJson() failed: ");
          Serial.println(error.c_str());
        } else{
          JsonObject object = doc.as<JsonObject>();
          assignConfigVariables(object);
        }
        writeLED(rValue, gValue, bValue);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
      //restablish wifi connection 
      connectToWifi(); 
      //once connection is restablished, loop begins again 
    }
    lastTime = millis();
  }
}