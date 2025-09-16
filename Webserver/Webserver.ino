#include <WiFi.h>
#include <WebServer.h>
#include <Ultrasonic.h>
#include <vector>

#define LED_BUILTIN 2
#define SENSOR_ECHO 12
#define SENSOR_TRIG 13
#define LEFT_MOTOR 27
#define RIGH_MOTOR 26

const char* ssid = "Robo_Desenhista";
std::vector<int> CommandList;

WebServer server(80);
Ultrasonic ultrasonic(SENSOR_TRIG, SENSOR_ECHO);

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
  Serial.println("\nConnected");
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
<meta name='viewport' content='width=device-width, initial-scale=1.0'>
<title>Controle do Robô</title>
<style>
  body {
    font-family: 'Arial', sans-serif;
    text-align: center;
    background-color: #f0f0f0;
    margin: 0;
    padding: 20px;
    box-sizing: border-box;
  }

  h2 {
    color: #333;
    font-size: 1.8em;
    margin-bottom: 20px;
  }

  .button-container {
    display: flex;
    flex-direction: column;
    align-items: center;
    gap: 15px;
  }

  button {
    width: 80%; /* Ajusta a largura dos botões para 80% da tela */
    max-width: 300px; /* Limita a largura máxima para não ficarem muito grandes em telas largas */
    padding: 15px 20px;
    font-size: 1.2em;
    color: white;
    background-color: #007BFF;
    border: none;
    border-radius: 8px;
    cursor: pointer;
    transition: background-color 0.3s ease;
  }

  button:hover {
    background-color: #0056b3;
  }

  .inline-buttons {
    display: flex;
    justify-content: center;
    gap: 10px; /* Espaço entre os botões */
  }

  .inline-buttons button {
    width: 48%; /* Cada botão ocupa metade da largura, com um pequeno espaço */
    max-width: 145px;
  }
</style>
</head>
<body>

<h2>Controle do Robô</h2>

<div class='button-container'>
  <button onclick="location.href='/forward'">Frente</button>
  
  <div class='inline-buttons'>
    <button onclick="location.href='/left'">Esquerda</button>
    <button onclick="location.href='/right'">Direita</button>
  </div>
  
  <button onclick="location.href='/start'">Start</button>
  <button onclick="location.href='/cancel'">Cancel</button>

  <div class='inline-buttons'>
    <button onclick="location.href='/undo'">Desfazer</button>
    <button onclick="location.href='/clear'">Limpar</button>
  </div>
</div>

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
    Serial.println("Lista de comandos vazia");
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
    Serial.println("Lista vazia, nada para remover");
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
      CommandList.clear();
      Serial.println("Todos os comandos executados");
      return;
    }

    if (ultrasonic.read() <= 30 && !waitingSensor) {
      digitalWrite(LEFT_MOTOR, LOW);
      digitalWrite(RIGH_MOTOR, LOW);
      Serial.println("Esperando liberação do sensor");
      waitingSensor = true;
      return;
    }

    if (ultrasonic.read() > 30 && waitingSensor) {
      Serial.println("Sensor liberado, continuando");
      waitingSensor = false;
      movementInProgress = false;
    }

    if (!movementInProgress && !waitingSensor) {
      int cmd = CommandList[currentCommandIndex];
      movementStartTime = millis();
      movementInProgress = true;

      switch (cmd) {
        case 0:
          digitalWrite(LEFT_MOTOR, HIGH);
          digitalWrite(RIGH_MOTOR, HIGH);
          Serial.println("Movendo: Frente");
          break;
        case 1:
          digitalWrite(RIGH_MOTOR, HIGH);
          Serial.println("Movendo: Esquerda");
          break;
        case 2:
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
