#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <BlynkSimpleEsp8266.h>

#include "DHT.h"
#define DHTPIN D1 // what digital pin we're connected to
#define DHTTYPE DHT11 // DHT 11
DHT dht(DHTPIN, DHTTYPE);
SimpleTimer timer;

String apiKey = "S1OTO80MFKP1MRUQ";
char auth[] = "MuNnL51d2CByCVKTJSuMFEEk0yJsKOuo";
char ssid[] = "Phong tro tang 2";
char pass[] = "minhhung@1234";
const char* server = "api.thingspeak.com";

WiFiClient client;

#define relay D0       
int rainPin = A0;
int sensor_analog;
float rain;      

void setup()
{   
    pinMode(relay, OUTPUT);
    Serial.begin(9600);
     
    Serial.println();
    Serial.print("Connecting to ");

    //Khởi tạo DHT11
    dht.begin(); 

    //ThingSpeak
    Serial.println(ssid);
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) 
    {
    delay(500);
    Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");

    //App Blynk
    Blynk.begin(auth, ssid, pass);
    timer.setInterval(2000, sendUptime);
}

void sendUptime()
{
     // Wait a few seconds between measurements.
     delay(2000);
    
    // Reading temperature or humidity takes about 250 milliseconds!
     // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
     float h = dht.readHumidity();
     // Read temperature as Celsius (the default)
     float t = dht.readTemperature();
     // Read temperature as Fahrenheit (isFahrenheit = true)
     float f = dht.readTemperature(true);
    
    // Check if any reads failed and exit early (to try again).
     if (isnan(h) || isnan(t) || isnan(f)) {
     Serial.println("Failed to read from DHT sensor!");
     return;
     }
    
    // Compute heat index in Fahrenheit (the default)
     float hif = dht.computeHeatIndex(f, h);
     // Compute heat index in Celsius (isFahreheit = false)
     float hic = dht.computeHeatIndex(t, h, false);

    sensor_analog = analogRead(rainPin);
    rain = ( 100 - ( (sensor_analog/1023.00) * 100 ) );
 
    Blynk.virtualWrite(V6, t);
    Blynk.virtualWrite(V5, h);
    Blynk.virtualWrite(V4, rain);
  
}

void loop()
{
  
  Blynk.run();
  timer.run();

  
  // Wait a few seconds between measurements.
  delay(2000);
  // Reading temperature or humidity takes about 250 milliseconds!
   // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
   float h = dht.readHumidity();
   // Read temperature as Celsius (the default)
   float t = dht.readTemperature();
   // Read temperature as Fahrenheit (isFahrenheit = true)
   float f = dht.readTemperature(true);
  
  // Check if any reads failed and exit early (to try again).
   if (isnan(h) || isnan(t) || isnan(f)) {
   Serial.println("Failed to read from DHT sensor!");
   return;
   }
  
  // Compute heat index in Fahrenheit (the default)
   float hif = dht.computeHeatIndex(f, h);
   // Compute heat index in Celsius (isFahreheit = false)
   float hic = dht.computeHeatIndex(t, h, false);

    Serial.print("Humidity: ");
     Serial.print(h);
     Serial.print(" %\t");
     Serial.print("Temperature: ");
     Serial.print(t);
     Serial.print(" *C ");
     
    sensor_analog = analogRead(rainPin);
    rain = ( 100 - ( (sensor_analog/1023.00) * 100 ) );

  
    Serial.print("Mositure : ");
    Serial.print(rain);
    Serial.println("%");
    digitalWrite(relay, HIGH);

    if (rain >30) {
        digitalWrite(relay, LOW);
        Serial.println("Ðộ ẩm đạt mức cho phép");
        delay(1000);
        Serial.println("ngừng tưới nước"); 
        }else{
           delay(2000);
           Serial.println("Bắt đầu tưới nước");
        }
        if (client.connect(server,80)) {
            String postStr = apiKey;              // add api key in the postStr string
            postStr += "&field1=";
            postStr += String(rain);    // add mositure readin
            postStr += "&field2=";
            postStr += String(h);                 // add tempr readin
            postStr += "&field3=";
            postStr += String(t);                  // add humidity readin
            postStr += "\r\n\r\n";
        
            client.print("POST /update HTTP/1.1\n");
            client.print("Host: api.thingspeak.com\n");
            client.print("Connection: close\n");
            client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
            client.print("Content-Type: application/x-www-form-urlencoded\n");
            client.print("Content-Length: ");
            client.print(postStr.length());           //send lenght of the string
            client.print("\n\n");
            client.print(postStr);                      // send complete string
            Serial.print("Moisture Percentage: ");
            Serial.print(rain);
            Serial.print(" C, Humidity: ");
            Serial.print(h);
            Serial.print("%. Temperature: ");
            Serial.print(t);
            Serial.println("%. Sent to Thingspeak.");
        }
}
