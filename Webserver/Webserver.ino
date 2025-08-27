#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Seu_SSID";
const char* password = "Seu_Password";

WebServer server(80);

void WifiConnection() {
  Serial.print("Connecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    digitalWrite(2, HIGH);
    delay(200);
    digitalWrite(2, LOW);
    delay(200);
  }
  Serial.println("Connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  server.on("/", handleRoot);
  server.begin();
  Serial.print("Server started.");
}

bool CheckWifiConnection() {
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(2, HIGH);
    return true;
  } else {
    digitalWrite(2, LOW);
    return false;
  }
}

void handleRoot() {
  server.send(200, "text/html",
              "<!DOCTYPE html>"
              "<html>"
              "<head><title>IoT</title></head>"
              "<body style='font-family: Arial; text-align: center; margin-top: 50px;'>"
              "<h1>IoT - Projeto Integrador</h1>"
              "</body></html>");
}

void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  WifiConnection();
}

void loop() {
  CheckWifiConnection();
  server.handleClient();
}
