#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

#define DHTPIN 4
#define DHTTYPE DHT22   // DHT 22 (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "Totalplay-27AA";
const char* password = "27AA57FA7Pb9GMKc";
const String firebaseURL = "https://a01411763-5ec8c-default-rtdb.firebaseio.com/"; // Firebase URL
int contador = 1; // Inicializa el contador

void setup() {
  Serial.begin(9600);
  conectarWiFi();
  dht.begin();
}

void conectarWiFi() {
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }

  Serial.println("Conexión WiFi exitosa");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

void enviarTemperatura(float temperatura) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String tempID = "temp" + String(contador); // Crea el identificador

    Serial.print("[HTTP] Iniciando solicitud PUT...\n");
    String url = firebaseURL + "Temperaturas/" + tempID + ".json"; // URL con el identificador
    http.begin(url); // Especifica la URL de Firebase
    http.addHeader("Content-Type", "application/json");

    // Envía la temperatura como un elemento independiente a Firebase
    String jsonData = "{\"temperatura\": " + String(temperatura) + "}";

    int httpCode = http.PUT(jsonData); // Envía la solicitud PUT con la temperatura

    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println("[HTTP] PUT exitoso, respuesta:");
      Serial.println(payload);
      contador++; // Incrementa el contador para el siguiente identificador
    } else {
      Serial.printf("[HTTP] PUT fallido, código de error: %d\n", httpCode);
    }

    http.end(); // Cierra la conexión
  } else {
    Serial.println("Error en la conexión WiFi");
  }
}

void loop() {
  delay(2000);

  float t = dht.readTemperature();

  if (isnan(t)) {
    Serial.println(F("Fallo al leer el sensor de temperatura!"));
    return;
  }

  Serial.print(F("Temperatura: "));
  Serial.print(t);
  Serial.println(F("°C"));

  // Envía la temperatura a Firebase
  enviarTemperatura(t);
}
