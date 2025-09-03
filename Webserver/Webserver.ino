#include <WiFi.h>
#include <WebServer.h>
#include <vector>

#define LED_BUILTIN 2
#define SENSOR 18
#define LEFT_MOTOR 4
#define RIGH_MOTOR 5

const char* ssid = "Robo_Desenhista";
std::vector<int> CommandList;

WebServer server(80);

void WifiConnection() {
  Serial.print("Connecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid);
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
  server.on("/forward", handleForward);
  server.on("/left", handleLeft);
  server.on("/right", handleRight);
  server.on("/start", handleStart);
  server.on("/undo", handleUndo);
  server.on("/clear", handleClear);

  server.begin();
  Serial.println("Server started.");
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

String getPage() {
  return R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset='UTF-8'>
<title>Controle do Robô</title>
</head>
<body style='text-align:center; font-family: Arial;'>
<h2>Controle do Robô</h2>

<button onclick="location.href='/forward'">Frente</button><br><br>
<button onclick="location.href='/left'">Esquerda</button>
<button onclick="location.href='/right'">Direita</button><br><br>

<button onclick="location.href='/start'">Start</button><br><br>
<button onclick="location.href='/undo'">Desfazer</button>
<button onclick="location.href='/clear'">Limpar</button>

</body>
</html>
)rawliteral";
}

void handleRoot() {
  server.send(200, "text/html", getPage());
}

void handleForward() {
  CommandList.push_back(0);
  Serial.println("Comando: Frente");
  server.send(200, "text/html", getPage());
}

void handleLeft() {
  CommandList.push_back(1);
  Serial.println("Comando: Esquerda");
  server.send(200, "text/html", getPage());
}

void handleRight() {
  CommandList.push_back(2);
  Serial.println("Comando: Direita");
  server.send(200, "text/html", getPage());
}

void handleStart() {
  Serial.println("Comando: Start");
  Movement();
  server.send(200, "text/html", getPage());
}

void handleUndo() {
  if(!CommandList.empty()){
    CommandList.pop_back();
    Serial.println("Comando: Undo");
  }
  else{
    Serial.println("Lista vazia, nada para remover.)");
  }
  server.send(200, "text/html", getPage());
}

void handleClear() {
  CommandList.clear();
  Serial.println("Comando: Clear");
  server.send(200, "text/html", getPage());
}

void Movement(){
  for(int i : CommandList){
    while(digitalRead(SENSOR)==HIGH){
      digitalWrite(LEFT_MOTOR, LOW);
      digitalWrite(RIGH_MOTOR, LOW);
    }
    switch(CommandList[i]){
      case 0:
        digitalWrite(LEFT_MOTOR, HIGH);
        digitalWrite(RIGH_MOTOR, HIGH);
        delay(1000);
        digitalWrite(LEFT_MOTOR, LOW;
        digitalWrite(RIGH_MOTOR, LOW);
        Serial.print("F");
        break;
      case 1:
        digitalWrite(RIGH_MOTOR, HIGH);
        delay(1000);
        digitalWrite(RIGH_MOTOR, LOW);
        Serial.print("L");
        break;
      case 2:
        digitalWrite(LEFT_MOTOR, HIGH);
        delay(1000);
        digitalWrite(LEFT_MOTOR, LOW);
        Serial.print("R");
        break;
    }
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SENSOR, INPUT);
  pinMode(LEFT_MOTOR, OUTPUT);
  pinMode(RIGH_MOTOR, OUTPUT);
  WifiConnection();
}

void loop() {
  CheckWifiConnection();
  server.handleClient();
}