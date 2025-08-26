#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Loft311";
const char* password = "Loft311#";

WiFiServer server(80);

void WifiConnection(){
  Serial.print("Connecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  while(WiFi.status()!=WL_CONNECTED){
    Serial.print(".");
    digitalWrite(2, HIGH);
    delay(200);
    digitalWrite(2, LOW);
    delay(200);
  }
  Serial.print("Connected.");
  Serial.print("IP Address: ");
  Serial.print(WiFi.localIP());
  server.begin();
}

bool CheckWifiConnection(){
  if(WiFi.status()==WL_CONNECTED){
    digitalWrite(2, HIGH);
    return true;
  }
  else{
    digitalWrite(2, LOW);
    return false;
  }
}

void setup(){

  Serial.begin(115200);
  pinMode(2, OUTPUT);
  WifiConnection();

}

void loop(){

  CheckWifiConnection();

}
