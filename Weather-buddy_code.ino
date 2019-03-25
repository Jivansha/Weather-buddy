#include "DHT.h"        // including the library of DHT11 temperature and humidity sensor
#define DHTTYPE DHT11   // DHT 11
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define dht_dpin D3   //D1
DHT dht(dht_dpin, DHTTYPE); 
#define OLED_RESET LED_BUILTIN
Adafruit_SSD1306 display(OLED_RESET);

String apiKey = "xxxxxxxxxxxxxxxxx";     //  Enter your Write API key from ThingSpeak

const char *ssid =  "name-WiFi";     // replace with your wifi ssid and wpa2 key
const char *pass =  "password_wifi";
const char* server = "api.thingspeak.com";

#define REPORTING_PERIOD_MS     10000     //changed from 1000
uint16_t tsLastReport = 0;

WiFiClient client;


float rem_dc(float hrp,float hr, float alpha = 0.95) {
  float wt;
  wt = hr + (alpha * hrp);
  hr = wt - hrp;
  return hr;
}


void setup(void)
{ 
  dht.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  
 
  display.display();
  Serial.begin(9600);
  Serial.println("Humidity and temperature \n\n");
   Serial.println("==========================================");
  // delay(700);
  // Clear the buffer.
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.setTextColor(WHITE);

  delay(1000);
  Serial.println("Connecting to ");
       Serial.println(ssid);
 
       WiFi.begin(ssid, pass);
 
      while (WiFi.status() != WL_CONNECTED) 
     {
            delay(500);
            Serial.print(".");
     }
      Serial.println("");
      Serial.println("WiFi connected");

}

////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
    float h = dht.readHumidity();
    float t = dht.readTemperature(); 
    float hp=dht.readHumidity();  
    float tp= dht.readTemperature();
         
    Serial.print("Current humidity = ");
    Serial.print(h);
    Serial.print("%  ");
    Serial.print("temperature = ");
    Serial.print(t); 
    Serial.println("C  ");

    display.setCursor(0,0);
    display.print("Humidity = ");
    display.print(h);
    display.print("\nTemperature = ");
    display.print(t);
    display.display(); //you have to tell the display to...display
    delay(2000);
    display.clearDisplay();

    delay(1000);

      if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
              h = dht.readHumidity();
              t = dht.readTemperature(); 
              h=rem_dc(hp,h);
              t=rem_dc(tp,t);

             
              tsLastReport = millis();
              
                         if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
                      {  
                            
                             String postStr = apiKey;


                               postStr +="&field1=";
                               postStr += String(t);
                               postStr +="&field2=";
                               postStr += String(h);           
                               postStr += "\r\n\r\n";


                               
 
                             client.print("POST /update HTTP/1.1\n");
                             client.print("Host: api.thingspeak.com\n");
                             client.print("Connection: close\n");
                             client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             client.print("Content-Type: application/x-www-form-urlencoded\n");
                             client.print("Content-Length: ");
                             client.print(postStr.length());
                             client.print("\n\n");
                             client.print(postStr);
 
                             Serial.println("..... Send to Thingspeak.");
                        }
          client.stop();
 
          Serial.println("Waiting...");
          hp=h;
          tp=t;
  
  // thingspeak needs minimum 15 sec delay between updates
  delay(10000);
   }
  delay(800);
}
