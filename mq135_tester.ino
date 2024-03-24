
#define mq135 A0


float hava_kalitesi, current_data;
void setup() {
  pinMode(mq135,INPUT);
  Serial.begin(9600);
}

void loop() {
  int read_sum = 0, anomaly_c = 0;
 
  for(int i = 0; i<10 ; i++){
    current_data = analogRead(mq135);
    read_sum += current_data;
    Serial.print(current_data);
    Serial.print("/");
    delay(600);

    //anomally detection
    if(current_data > 200){
      anomaly_c++;
    }

    //checking if it is really an anomally or a real change
    if(anomaly_c > 4){
      Serial.print("danger: air quality worsened anormally");
      break;
    }
  }
  
  hava_kalitesi = read_sum/10;
  
  current_data = analogRead(mq135);
  Serial.print("Odadaki Gaz Değeri : ");
  Serial.print(current_data);
  Serial.println(" PPM");
  delay(500);
  
   
}
