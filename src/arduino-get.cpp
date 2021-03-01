#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClientSecureBearSSL.h>
#include <Wire.h>
#include <WiFiClient.h>
#include "ArduinoJson.h"

#define SSID "Ayn Rand - 2.4G"
#define PASSWD "johngalt"
#define pino_sinal_analogico A0

ESP8266WiFiMulti WiFiMulti;

const char fingerprint[] PROGMEM =  "94 FC F6 23 6C 37 D5 E7 92 78 3C 0B 5F AD 0C E4 9E FD 9E A8";
char json[400] = {0};

DynamicJsonDocument doc(1024);

int leitura_sensor_umidade;

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Set timer to 20 seconds (20000)
unsigned long timerDelay = 20000;



void setup() {
  pinMode(pino_sinal_analogico, INPUT);
  
  Serial.begin(9600);
  Serial.println("\n Setup on...");

  //2 - iniciamos a conexão WiFi...
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(SSID, PASSWD);
}


void loop() {
if ((millis() - lastTime) > timerDelay) {
  
   leitura_sensor_umidade = analogRead(pino_sinal_analogico);
   
  Serial.print("\nLeitura umidade: ");
  Serial.println(leitura_sensor_umidade);
   
  Serial.println("loop started...");
        if ((WiFiMulti.run() == WL_CONNECTED)){
          std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
          client->setFingerprint(fingerprint);
          Serial.println("connected...");
          //WiFiClient client;

          HTTPClient http;

        //3 - iniciamos a URL alvo, pega a resposta e finaliza a conexão
        if (http.begin(*client,"https://garduino-galtec.herokuapp.com/api/jardim/sendHumidity/")){
          Serial.println("http.begin ok");
        }

        String valor = String(leitura_sensor_umidade);
        Serial.print("humidity: ");
        Serial.println(valor);

       
        http.addHeader("Content-Type", "application/json");
        
        int httpCode = http.POST("{\"humidity\":"+valor+"}");  

        Serial.print("HTTP Response code: ");
        Serial.println(httpCode);
        if (httpCode > 0) { //Maior que 0, tem resposta a ser lida
            String payload = http.getString();
            Serial.println(httpCode);
            Serial.println(payload);
            deserializeJson(doc, payload);
            JsonObject obj = doc.as<JsonObject>();
            bool watering = obj[String("watering")];
            int duration = obj[String("duration")];
            bool interval = obj[String("interval")];
            String next_watering_avaliable = obj[String("next_watering")];
            Serial.print("\nwatering: ");
            Serial.println(watering);
            Serial.print("\nduration: ");
            Serial.println(duration);
            Serial.print("\nnext Watering time avaliable: ");
            Serial.println(next_watering_avaliable);
            Serial.print("\nnext interval avaliable: ");
            Serial.println(interval);
        }
        else {
          Serial.println(httpCode);
            Serial.println("Falha na requisição");
        }
        http.end();
        }
         lastTime = millis();
        }
   
}