#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>
#include <stdlib.h>
#include <string.h>

// ----------------------------------------------------------sensor libs ----------------------------------------------------
#include <Wire.h>
#include "DHT.h"

//---------------------------------------------------------------------------------------------------------------------------

/* PIN LAYOUT OF ESP12E 

D1  GPIO 5 
D2  GPIO 4
D3  GPIO 0
D4  GPIO 2
D5  GPIO 14
D6  GPIO 12
D7  GPIO 13
*/
// ----------------------------------------------------------pins constants---------------------------------------------------
#define DHTPIN 4
#define DHTTYPE DHT11 
#define mq135 A0


// ----------------------------------------------------------wifi credentials-------------------------------------------------

//Config variables
const char *ssid = "your wifi name";
const char *password = "your pasword";
const char *mqtt_server = "test.mosquitto.org";  //could be another mqtt server provider but mqtt libraries may differ than the ones which are used in this project
const uint16_t mqtt_server_port = 1883;          
 
//---------------------------------------------------------------------------------------------------------------------------

// globals

float current_data;

DHT dht(DHTPIN, DHTTYPE);

WiFiClient wifi_client;
PubSubClient mqttClient(wifi_client);

const char *mqtt_topic_out = "room air checker";


// connect to wifi
void setup_wifi() {
  //print info about network connection
  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(ssid);
  Serial.print(" with password ");
  Serial.println(password);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
}

//function to run when a message comes from the topic

void callback(char *topic, byte *payload, unsigned int length) {
// this part will be executed at arrival of a message
}

// connect client to the broker
void connect() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    String mqttClientId = "";
    if (mqttClient.connect(mqttClientId.c_str())) {
      Serial.println("connected");
      mqttClient.subscribe(mqtt_topic_out);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" will try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
//---------------------------------------------dht-mq135 sensor-----------------------------
  Serial.begin(9600);

  dht.begin();
  pinMode(mq135,INPUT);


//--------------------------------------------wifi-mqtt-------------------------------------


  Serial.begin(115200);
  setup_wifi();
  mqttClient.setServer(mqtt_server, mqtt_server_port);
  mqttClient.setCallback(callback);
}

void loop() {
  if (!mqttClient.connected()) {
    connect();
  }
  mqttClient.loop();


//--------------------------------------------sensor-read-------------------------------------

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  int air_quality = 0,air_sum = 0, h_sum = 0, t_sum = 0, anomaly_c = 0, anomaly = 0, humidity= 3, h_avg, t_avg;
  int i = 0, j = 0;

  // reads dht 10 times in a row and takes their sum for avg value calculation 
  while(i<10){

    h= dht.readHumidity();
    t= dht.readTemperature();

    h_sum += h;
    t_sum += t;
    

    Serial.print(h);
    Serial.print("/");

    Serial.print(t);
    Serial.print("/");
    
    delay(600);
    
    i++;
  }
  
  Serial.print("\n");
  
  // reads mq135 10 times in a row and takes their sum for avg value calculation 
  while(j<10){
    current_data = analogRead(mq135);
    air_sum += current_data;
    

    Serial.print(current_data);
    Serial.print("/");

    //anomally detection
    if(current_data > 350){
      anomaly_c++;
    }

    //checking if it is really an anomally or a real change
    if(anomaly_c > 4){
      anomaly = 1;
      break;
    }

    j++;
  }
  //--------------------------------------------analysis-----------------------------------------

  air_quality = air_sum/j;
  h_avg = h_sum/i;
  t_avg = t_sum/i;

  //humidity analysis
    if(h_avg>30 && h_avg<70){
      humidity = 1;
    }
    else if(h_avg<=30){
      humidity = 0;
    }
    else{
      humidity = 2;
    }

  switch(humidity){
    case 0:
      Serial.print("\n    too dry");
      break;
    case 1:
      Serial.print("\n    humidity normal");
      break;
    case 2:
       Serial.print("\n   too humid");
       break;
    case 3:
       break;
  }

  Serial.print("\n Average temperature and humidity value in the room : ");
  Serial.print(t_avg);
  Serial.print("C, %");
  Serial.print(h_avg);
  
  
  Serial.print("\n Gas value in the room : ");
  Serial.print(air_quality);
  Serial.println(" PPM");
  

//String(val)).c_str()

  //--------------------------------------------mqtt-publish-----------------------------------------

  delay(2500);
  mqttClient.publish(mqtt_topic_out, ("Humidity: " + String(h_avg) + "  Temperature: " + String(t_avg)).c_str() );
  switch(humidity){
    case 0:
      mqttClient.publish(mqtt_topic_out,"too dry");
      break;
    case 1:
       mqttClient.publish(mqtt_topic_out,"humidity normal");
       break;
    case 2:
        mqttClient.publish(mqtt_topic_out,"too humid");
        break;
    case 3:
        break;
  }
  
  delay(2500);
  if(anomaly){
    mqttClient.publish(mqtt_topic_out, ("Air quality is low: " + String(air_quality) + "PPM").c_str());
  }
  
  else{
    mqttClient.publish(mqtt_topic_out, ("Air quality is normal: " + String(air_quality) + "PPM").c_str());
    }
}
