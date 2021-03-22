 /*********
iJim Silvia PID code
WIFI Version - needs HTML page uploading to EPS32 via sketch data upload under tools!
WIFI/WEB Server/Graph all taken from this example:
https://randomnerdtutorials.com/esp32-esp8266-plot-chart-web-server/
You must have index.html in a folder named data, which sits in the same folder as the sketch.
*********/

// Import required libraries
  #include <WiFi.h>
  #include <ESPAsyncWebServer.h>
  #include <SPIFFS.h>
  #include <PIDController.h>
  #include "max6675.h"

// Objects
PIDController pid; // Create an instance of the PID controller class, called "pid"
String sliderValue = "100";
int thermoDO = 26;
int thermoCS = 25;
int thermoCLK = 33;
int LLevel = 19; //Not fully implemented yet.

float t; // current temperature
int output; //output of PID
float t_past; // Previous temperature measurment.
const char* PARAM_INPUT = "value";

int BrewTemp = 102; //initial value - this is the boiler wall temp, not water!
int SteamTemp = 130; //initial value
int BrewSteam = 0; //initial value
int WIFIlag;


// Replace with your network credentials
const char* ssid = "H298A"; //It took me a long time to realise it will not connect if your WIFI name is too long - I forget how many characters, if you are having connection problems try rename.
const char* password = "WIFI PASSWORD HERE";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

// Prepare temp measurment to send to web server.
String readDSTemperature() {
    return String(t);
}

String readSetpoint() {
  if (BrewSteam == 0) {
    return String(BrewTemp);
  }
  else{
    return String(SteamTemp);
  }
}

String readPID() {
    return String(output / 20);
}

void setup(){

  // Serial port for debugging purposes
  Serial.begin(115200);
 // sensors.begin();
  delay(250);
  pinMode(23, OUTPUT); //RELAY
  pinMode(LLevel, INPUT); // Not implemented yet.


  t = thermocouple.readCelsius(); //take an initial reading
  t_past = t; //set inital reading to the historic value on first run.
  
  pid.begin();          // initialize the PID instance
  pid.setpoint(BrewTemp);    // The "goal" the PID controller tries to "reach"
  pid.tune(600, 1, 30);    // Tune the PID, arguments: kP, kI, kD: THESE ARE VERY CONSERVATIVE.
  pid.limit(0, 2000);    // Limit the PID output between 0 and 255, this is important to get rid of integral windup!
  
  
  // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  WiFi.begin(ssid, password);
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Waiting to connect to WiFi..");
    delay(5000);
  }
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Sorry, no WIFI found..");
    WIFIlag = 1;
  }
  else {
      // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
  WIFIlag = 0;
  }
 
if (WIFIlag == 0){
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html");
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    delay(100);
    request->send_P(200, "text/plain", readDSTemperature().c_str());
    delay(100);
    });
    server.on("/setpoint", HTTP_GET, [](AsyncWebServerRequest *request){
    delay(100);
    request->send_P(200, "text/plain", readSetpoint().c_str());
    delay(100);
    });

   server.on("/PID", HTTP_GET, [](AsyncWebServerRequest *request){
    delay(100);
    request->send_P(200, "text/plain", readPID().c_str());
    delay(100);
    });

server.on("/slider", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    // GET input1 value on <ESP_IP>/slider?value=<inputMessage>
    if (request->hasParam(PARAM_INPUT)) {
      inputMessage = request->getParam(PARAM_INPUT)->value();
      sliderValue = 100;//inputMessage;
      //ledcWrite(ledChannel, sliderValue.toInt());
    }
    else {
      inputMessage = "No message sent";
    }
    
  int commaIndex = inputMessage.indexOf(',');
  //  Search for the next comma just after the first
  int secondCommaIndex = inputMessage.indexOf(',', commaIndex + 1);
  String BrewSteamSTR = inputMessage.substring(0, commaIndex);
  String BrewTempSTR = inputMessage.substring(commaIndex + 1, secondCommaIndex);
  String SteamTempSTR = inputMessage.substring(secondCommaIndex + 1); // To the end of the string

  BrewSteam = BrewSteamSTR.toInt();
  BrewTemp = BrewTempSTR.toInt();
  SteamTemp = SteamTempSTR.toInt();

// Serial prints for debugging, can be removed.
    Serial.println("New settings:");
    Serial.print("Brew or Steam: ");
    Serial.println(BrewSteam);
    Serial.print("Brew temp: ");
    Serial.println(BrewTemp);
    Serial.print("Steam temp: ");
    Serial.println(SteamTemp);
    
    request->send(200, "text/plain", "OK");
    
    if (BrewSteam ==1){
      pid.setpoint(SteamTemp);
          }
          else {
            pid.setpoint(BrewTemp);
          }    
   });

  

  // Start server
  server.begin();
}
}


void loop(){
 int Lval = digitalRead(LLevel);
 Serial.println(Lval);

 //temperature reads with big delays, this is overkill, I will limit to 3 in future version. 
  float temperatureC1 = thermocouple.readCelsius();
  delay(200);
  float temperatureC2 = thermocouple.readCelsius();
  delay(200);
  float temperatureC3 = thermocouple.readCelsius();
  delay(200);
  float temperatureC4 = thermocouple.readCelsius();
  delay(200);
  float temperatureC5 = thermocouple.readCelsius();
  delay(200);
  float temperatureC6 = thermocouple.readCelsius();
  delay(200);
  float temperatureC7 = thermocouple.readCelsius();
  delay(200);
  float temperatureC8 = thermocouple.readCelsius();
  delay(200);
  
  t = (temperatureC1 + temperatureC2 + temperatureC3 + temperatureC4 + temperatureC5 + temperatureC6 + temperatureC7 + temperatureC8)/8; //take the average temperature from 8 reads

///FULL POWER < 88C - this helps a lot!
 if (t > 1 && t <= 88 ) { 
  if (t < t_past + 15) {  //I noticed an occiaisal temperature spike (of like 2000C, clearly a read error), this sent the PID crazy, so I have this if statement to ignore big jumps, the machine will never jump 15C in a few seconds. 
          digitalWrite(23, HIGH);
          Serial.println("LOW temp - FULL POWER");
          output = 2000; //change variable to report on web page.
          delay(2000);
          t_past = t;
  } else {
    Serial.println("BIG TEMP SPIKE DETECTED");
    digitalWrite(23, LOW);
    delay(2000);
  }
 }

// code below is the main PID loop, it runs for 2 seconds, it's used between 88 and 150C, I think 2 seconds is too slow based off comments on forum.
 if (t > 88 && t < 150 ) { 
  if (t < t_past + 15) { 
    output = pid.compute(t - 0.2); 
        if (output > 100 && output < 2001) {
          Serial.print("PID Running");
          Serial.println(output);
          digitalWrite(23, HIGH);
          delay(output);   // Keep SSR on for duration specified by PID.
          digitalWrite(23, LOW);
          delay(2000-output); //Switches SSR off for 2 seconds minus duration of PID.
          t_past = t;
            }
    } else {
    Serial.println("BIG TEMP SPIKE DETECTED");
    digitalWrite(23, LOW);
    delay(2000);
  }
 }
  
  
}
