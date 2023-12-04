#include <WiFi.h>
#include <ModbusIP_ESP8266.h>
#include <OneWire.h>
#include "DHT.h"

const int REG = 16;               // Modbus Hreg Offset
IPAddress remote(10, 14, 250, 20);  // Address of Modbus Slave device
#define DHTPIN 4
#define DHTTYPE DHT22   // DHT 22 (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);

ModbusIP mb;  //ModbusIP object

void setup() {
  pinMode(35, INPUT);
  pinMode(32, OUTPUT);
  Serial.begin(9600);
  WiFi.begin("SmartFactory_Red");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  dht.begin();
  mb.client();
  blinkLED();
}

void blinkLED(){
  for (int i = 0; i <= 5; i++){

    digitalWrite(32, HIGH);
    delay(100);
    digitalWrite(32, LOW);
    delay(100);
  }
}

void blinkLED2(){
  for (int i = 0; i <= 0; i++){

    digitalWrite(32, HIGH);
    delay(200);
    digitalWrite(32, LOW);
    delay(200);
  }
}

uint16_t res = 0;
int first = true;
float t;
void loop() {
  t = dht.readTemperature();
  if (isnan(t)) {
    Serial.println(F("Fallo al leer el sensor de temperatura!"));
    return;
  }

  Serial.print(F("Temperatura: "));
  Serial.print(t);
  Serial.println(F("°C"));

  if (mb.isConnected(remote)) {   // Check if connection to Modbus Slave is established
    mb.writeHreg(remote, REG, t);
    Serial.println("Value sent succesfully!");
    blinkLED2();
    //mb.readHreg(remote, REG, &res);  // Initiate Read Coil from Modbus Slave
  } else {
    mb.connect(remote, 502);
    Serial.print(mb.isConnected(remote));           // Try to connect if no connection
  }
  mb.task();                      // Common local Modbus task
  delay(1000);                     // Pulling interval
}
