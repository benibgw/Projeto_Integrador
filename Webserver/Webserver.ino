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

bool inMovement = false;
int currentCommandIndex = 0;
unsigned long movementStartTime = 0;
bool movementInProgress = false;
bool waitingSensor = false;

void WifiConnection() {
  Serial.print("Connecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
  }
  Serial.println("\nConnected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/forward", handleForward);
  server.on("/left", handleLeft);
  server.on("/right", handleRight);
  server.on("/cancel", handleCancel);
  server.on("/start", handleStart);
  server.on("/undo", handleUndo);
  server.on("/clear", handleClear);

  server.begin();
  Serial.println("Server started.");
}

bool CheckWifiConnection() {
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, HIGH);
    return true;
  } else {
    WifiConnection();
    digitalWrite(LED_BUILTIN, LOW);
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
<button onclick="location.href='/cancel'">Cancel</button><br><br>
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
  if (!CommandList.empty()) {
    inMovement = true;
    currentCommandIndex = 0;
    movementInProgress = false;
    waitingSensor = false;
  } else {
    Serial.println("Lista de comandos vazia.");
  }
  server.send(200, "text/html", getPage());
}

void handleCancel() {
  Serial.println("Comando: Cancel");
  inMovement = false;
  movementInProgress = false;
  digitalWrite(LEFT_MOTOR, LOW);
  digitalWrite(RIGH_MOTOR, LOW);
  server.send(200, "text/html", getPage());
}

void handleUndo() {
  if (!CommandList.empty()) {
    CommandList.pop_back();
    Serial.println("Comando: Undo");
  } else {
    Serial.println("Lista vazia, nada para remover.");
  }
  server.send(200, "text/html", getPage());
}

void handleClear() {
  CommandList.clear();
  Serial.println("Comando: Clear");
  server.send(200, "text/html", getPage());
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

  if (inMovement) {
    if (currentCommandIndex >= CommandList.size()) {
      inMovement = false;
      currentCommandIndex = 0;
      Serial.println("Todos os comandos executados.");
      return;
    }

    if (digitalRead(SENSOR) == HIGH && !waitingSensor) {
      digitalWrite(LEFT_MOTOR, LOW);
      digitalWrite(RIGH_MOTOR, LOW);
      Serial.println("Esperando liberação do sensor...");
      waitingSensor = true;
      return;
    }

    if (digitalRead(SENSOR) == LOW && waitingSensor) {
      Serial.println("Sensor liberado. Continuando...");
      waitingSensor = false;
      movementInProgress = false;
    }

    if (!movementInProgress && !waitingSensor) {
      int cmd = CommandList[currentCommandIndex];
      movementStartTime = millis();
      movementInProgress = true;

      switch (cmd) {
        case 0
          digitalWrite(LEFT_MOTOR, HIGH);
          digitalWrite(RIGH_MOTOR, HIGH);
          Serial.println("Movendo: Frente");
          break;
        case 1:
          digitalWrite(RIGH_MOTOR, HIGH);
          Serial.println("Movendo: Esquerda");
          break;
        case 2
          digitalWrite(LEFT_MOTOR, HIGH);
          Serial.println("Movendo: Direita");
          break;
      }
    }

    if (movementInProgress && millis() - movementStartTime >= 1000) {
      digitalWrite(LEFT_MOTOR, LOW);
      digitalWrite(RIGH_MOTOR, LOW);
      movementInProgress = false;
      currentCommandIndex++;
    }
  }
}