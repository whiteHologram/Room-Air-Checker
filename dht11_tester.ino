#include "DHT.h"
#define DHTPIN 4
#define DHTTYPE DHT11 

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  dht.begin();

}

void loop() {
  // put your main code here, to run repeatedly:
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print(F("\nHumidity: "));
  Serial.print(h);
  Serial.print(F("  %  Temperature: "));
  Serial.print(t);

  int h_sum = 0, t_sum = 0, humidity= 3, h_avg, t_avg;

  for(int i = 0; i<10 ; i++){

    h_sum += dht.readHumidity();
    t_sum += dht.readTemperature();
  }

  h_avg = h_sum/10;
  t_avg = t_sum/10;

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
      Serial.print("\n too dry");
      break;
    case 1:
       Serial.print("\n humidity normal");
       break;
    case 2:
        Serial.print("\n too humid");
        break;
    case 3:
        break;
  }
  delay(2500);
  Serial.print("\n Odadaki nem sıcaklık ort değeri : ");
  Serial.print(h_avg, t_avg);
}
