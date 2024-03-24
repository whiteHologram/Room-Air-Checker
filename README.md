# Room-Air-Checker
this is an arduino project which aims an iot indoors air checker
used sensors for this project:
  mq135: air quality sensor which can detect dangerous gases such as ammonia, benzene, sulfur, carbon dioxide, smoke, and other harmful gases as ppm value
  dht11: temperature and humidity sensor
used wifi cart is esp12 on nodeMCU module which is calles as esp8266
targeted outcome in this project is being able to have messages or a signal from mobile phone when room checker detect any dangerous levels of humidity temperature or gas as being able to identify it as a anomaly 
for power management, system doesn't send any data package before collecting data for long period of time and processing it beforehand
code also includes reading error control, if it is a broken sensor or what they detect is a true anomaly in air quality
in this project it is accepted that 400 ppm and more indicates poor air quality

for best results from mq135 sensor it is suggested in datasheet that leaving the sensor active for 24 hours before using it for any detection
