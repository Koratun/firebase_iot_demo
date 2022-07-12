/*
 ESP8266 --> ThingSpeak Channel
 
 This sketch sends the Wi-Fi Signal Strength (RSSI) of an ESP8266 to a ThingSpeak
 channel using the ThingSpeak API (https://www.mathworks.com/help/thingspeak).
 
 Requirements:
 
   * ESP8266 Wi-Fi Device
   * Arduino 1.8.8+ IDE
   * Additional Boards URL: http://arduino.esp8266.com/stable/package_esp8266com_index.json
   * Library: esp8266 by ESP8266 Community
   * Library: ThingSpeak by MathWorks
 
 ThingSpeak Setup:
 
   * Sign Up for New User Account - https://thingspeak.com/users/sign_up
   * Create a new Channel by selecting Channels, My Channels, and then New Channel
   * Enable one field
   * Enter SECRET_CH_ID in "secrets.h"
   * Enter SECRET_WRITE_APIKEY in "secrets.h"

 Setup Wi-Fi:
  * Enter SECRET_SSID in "secrets.h"
  * Enter SECRET_PASS in "secrets.h"
  
 Tutorial: http://nothans.com/measure-wi-fi-signal-levels-with-the-esp8266-and-thingspeak
   
 Created: Feb 1, 2017 by Hans Scharler (http://nothans.com)
*/

#include "IOXhop_FirebaseESP32.h"
#include "secrets.h"

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

#include <WiFi.h>
#include <M5StickCPlus.h>
#include <ArduinoJson.h>


#define LED_BUILTIN 10

float accX = 0.0F;
float accY = 0.0F;
float accZ = 0.0F;

float gyroX = 0.0F;
float gyroY = 0.0F;
float gyroZ = 0.0F;

float pitch = 0.0F;
float roll  = 0.0F;
float yaw   = 0.0F;

char ssid[] = SECRET_SSID;   // your network SSID (name)
char pass[] = SECRET_PASS;   // your network password
String FIREBASE_HOST = "fir-test-803bb-default-rtdb.firebaseio.com/";
String FIREBASE_AUTH = "g5rROw5diwWUpEPoRkvvOvZrthnPn2fQdhMCGX69";
int keyIndex = 0;            // your network key index number (needed only for WEP)
WiFiClient  client;

void setup() {

  WiFi.mode(WIFI_STA);

  pinMode(LED_BUILTIN, OUTPUT);
  //High is off and Low is on.
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(SECRET_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected.");

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.setString("LED_STATUS", "OFF");
  
  M5.begin();
  M5.IMU.Init();
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(80, 15);
  M5.Lcd.println("IMU TEST");
  M5.Lcd.setCursor(30, 30);
  M5.Lcd.println("  X       Y       Z");
  M5.Lcd.setCursor(30, 70);
  M5.Lcd.println("  Pitch   Roll    Yaw");
}


float maxMag = 0;
long timer = 0;
long thingspeakTimer = 0;

void loop() {

  M5.IMU.getGyroData(&gyroX,&gyroY,&gyroZ);
  M5.IMU.getAccelData(&accX,&accY,&accZ);
  M5.IMU.getAhrsData(&pitch,&roll,&yaw);
  
  M5.Lcd.setCursor(30, 40);
  M5.Lcd.printf("%6.2f  %6.2f  %6.2f      ", gyroX, gyroY, gyroZ);
  M5.Lcd.setCursor(170, 40);
  M5.Lcd.print("o/s");
  M5.Lcd.setCursor(30, 50);
  M5.Lcd.printf(" %5.2f   %5.2f   %5.2f   ", accX, accY, accZ);
  M5.Lcd.setCursor(170, 50);
  M5.Lcd.print("G");
  M5.Lcd.setCursor(30, 80);
  M5.Lcd.printf(" %5.2f   %5.2f   %5.2f   ", pitch, roll, yaw);

  float accelMag = sqrt(pow(accX, 2) + pow(accY, 2) + pow(accZ, 2));

  /*if(accelMag > maxMag){
    maxMag = accelMag;
    digitalWrite(LED_BUILTIN, LOW);
    timer = 3000 + millis();
  }*/

  String firestatus = Firebase.getString("LED_STATUS");
  
  if(firestatus == "OFF"){
    Serial.println("Led turned off");
    digitalWrite(LED_BUILTIN, HIGH);
  }else if(firestatus == "ON"){
    Serial.println("Led turned on");
    digitalWrite(LED_BUILTIN, LOW);
  }else{
    Serial.println("Wrong credentials!");
  }
  
  /*if(timer !=0 && millis() > timer){
    digitalWrite(LED_BUILTIN, HIGH);
    timer = 0;
  }*/

  M5.Lcd.setCursor(30, 95);
  M5.Lcd.printf("Max Acceleration: %5.2f", maxMag);

  // Write value to Field 2 of my ThingSpeak Channel
  /*if(thingspeakTimer < millis()){
    
    int httpCode = ThingSpeak.writeField(myChannelNumber, 2, maxMag, myWriteAPIKey);
  
    if (httpCode == 200) {
      Serial.println("Channel write successful.");
    }
    else {
      Serial.println("Problem writing to channel. HTTP error code " + String(httpCode));
    }

    thingspeakTimer = 20000 + millis();
  }*/
}
