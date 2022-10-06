#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>


const char* ssid = "ssid"; //input ssid here
const char* password = "password"; //input password to network here 

//Your Domain name with URL path or IP address with path
String serverName = "https://qc-incubator.herokuapp.com/esp32/";

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

int tod = -1; // "20:19:29" -> int = 73169 (sec)
int sunrise_start = -1; // "08:00" -> int -> 28800 (sec)
int sunset_start = -1; // "20:00" -> int -> 72000 (sec)
int duration = -1; // 60 mins -> 3600 (sec)

enum stage {pre_sunrise, sunrise, lights_on, sunset, post_sunset};

//for checking 
//int timeCheck = 28800;

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
//no longer needed because getting second value already calculated from heroku but keeping in case needed again 
int timeInSeconds(String time){
  int firstColon = time.indexOf(':');
  int hour = time.substring(0,firstColon).toInt();
  int minutes =  time.substring(firstColon+1,firstColon+3).toInt();

  int seconds = time.substring(time.indexOf(':',firstColon+1)+1).toInt();
  return hour*60*60+minutes*60+seconds;
}


void assignConfigVariables(JsonObject doc){
    // String todStr = doc["tod"]; // "20:29:39"
    // String sunriseStr = doc["sunrise"]; // "08:00"
    // String sunsetStr = doc["sunset"]; // "20:00"
    // tod = timeInSeconds(todStr);
    //testing time skip 
    //tod = 25200 + timeCheck;
    //tod = 71998 + timeCheck;
    //timeCheck = timeCheck + 60; //increment by 1 minute each time 
    //tod = timeCheck; 

    // sunrise_start = timeInSeconds(sunriseStr);
    // sunset_start = timeInSeconds(sunsetStr);

    tod = doc["tod"];
    sunrise_start = doc["sunrise"];
    sunset_start = doc["sunset"];
    duration = doc["duration"]; // 60
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

bool variablesInitialized(){
  String s = ""; 
  bool notInit = false;
  if(tod < 0){
    s.concat("tod uninitialized");
    notInit = true;
  }
  if(sunrise_start < 0){
    s.concat("sunrise uninitialized");
    notInit = true;
  }
    if(sunset_start < 0){
    s.concat("sunrise uninitialized");
    notInit = true;
  }
    if(duration < 0){
    s.concat("sunrise uninitialized");
    notInit = true;
  }

  //return false if any variables was not initialized otherwise return true
  if(notInit)
    return false;
  else 
    return true;
}

//updates system time and brightness using existing variables if disconnected from wifi or heroku 
void assignIfDisconnected(){
    tod = (millis()-lastTime)/1000+tod; //increments tod by two seconds 

    //if tod is greater than 24:00, resets to 00:00+time since last ~ 00:02 first time 
    if(tod>86400)
      tod = tod-86400;
    Serial.print("Difference in time since last call: ");
    Serial.println((millis()-lastTime)/1000);
    Serial.print("tod in time: ");
    Serial.println(tod);
    float brightness = getBrightness(tod, duration);
    Serial.print("Brightness: ");
    Serial.println(brightness);
    rValue = (int)(rValue*brightness); // 0
    gValue = (int)(gValue*brightness); // 255
    bValue = (int)(bValue*brightness); // 255
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

void getInitialConfigFromHeroku(){
     HTTPClient http;

    String serverPath = serverName + "config";

    // Your Domain name with URL path or IP address with path
    http.begin(serverPath.c_str());
    
    // Send HTTP GET request
    int httpResponseCode = http.GET();
    if(httpResponseCode!=201){
      Serial.println("Unable to connect to heroku for initial setup, attempting to reconnect");
    }
    while(httpResponseCode!=201){
      delay(500);
      Serial.print(".");
      httpResponseCode = http.GET();
    }
    if (httpResponseCode>0) {
      Serial.println("\nSucessfully connected to heroku");

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

      //end connection
      http.end();

      if(!variablesInitialized()){
        //reattempt to connect to heroku if failed at this juncture
        Serial.println("Config variables were not properly recieved for initial setup, attempting to reconnect");
        getInitialConfigFromHeroku();
      } else{
        Serial.println("Sucessfully configured config variables on system, turning lights on to config settings");
        writeLED(rValue, gValue, bValue);
      }
    }
}


void setup() {
  Serial.begin(115200); 

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  connectToWifi();
  Serial.println("Timer set to 2 seconds (timerDelay variable), it will take 2 seconds before first attempting connection to heroku.");
  getInitialConfigFromHeroku();

  /*time format to seconds testing 
  Serial.print("Time check");
  Serial.println(timeInSeconds("14:59:59"));
    Serial.print("Time check");
  Serial.println(timeInSeconds("14:59:06"));
    Serial.print("Time check");
  Serial.println(timeInSeconds("14:1:5"));
      Serial.print("Time check");
  Serial.println(timeInSeconds("14:1:54"));
  */

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

          Serial.print("Running off of previous system variables  ");
          assignIfDisconnected();

        } else{
          JsonObject object = doc.as<JsonObject>();
          assignConfigVariables(object);
          writeLED(rValue, gValue, bValue);
        }
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
        Serial.println("Could not restablish heroku connection, resorting to updating with current system data until connection can be restablished");
        assignIfDisconnected();
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
      //restablish wifi connection 
      WiFi.begin(ssid, password); //attempt once to restablish connection 
      if(WiFi.status() != WL_CONNECTED){
        Serial.println("Could not restablish wifi connection, resorting to updating with current system data");
        assignIfDisconnected();
      } else{
        Serial.println("Restablished wifi connection");
      }
    }
    lastTime = millis();
  }
  //if millis() overflows and becomes 0 again, lastTime will still be near max of long, and in that case, set last to millis() to reset cand continue 
  else if(lastTime>millis()){
    lastTime = millis();
  }
}