#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Ultrasonic.h>
#include <vector>
#include <ESPmDNS.h>

#define LED_BUILTIN 2
#define SENSOR_ECHO 12
#define SENSOR_TRIG 13
#define RIGHT_MOTOR_PWM 14
#define LEFT_MOTOR_PWM 18

const char* ssid = "Robo_Desenhista";
std::vector<int> CommandList;

WebServer server(80);
Ultrasonic ultrasonic(SENSOR_TRIG, SENSOR_ECHO);

bool inMovement = false;
int currentCommandIndex = 0;
unsigned long movementStartTime = 0;
bool movementInProgress = false;
bool waitingSensor = false;

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
  CommandList.clear();
  analogWrite(LEFT_MOTOR_PWM, 0);
  analogWrite(RIGHT_MOTOR_PWM, 0);
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

void ApInit() {
  Serial.print("Init AP. SSID: ");
  Serial.print(ssid);
  WiFi.softAP(ssid);
  if (MDNS.begin("Robo_Desenhista")) {
    Serial.println("mDNS : http://Robo_Desenhista.local/");
  } else {
    Serial.println("Erro ao iniciar mDNS");
  }

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

void CheckApState() {
  if (WiFi.softAPgetStationNum() >= 0) {
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LEFT_MOTOR_PWM, OUTPUT);
  pinMode(RIGHT_MOTOR_PWM, OUTPUT);
  ApInit();
}

void loop() {
  CheckApState();
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
      analogWrite(LEFT_MOTOR_PWM, 0);
      analogWrite(RIGHT_MOTOR_PWM, 0);
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
          delay(200);
          analogWrite(LEFT_MOTOR_PWM, 255);
          analogWrite(RIGHT_MOTOR_PWM, 255);
          Serial.println("Movendo: Frente");
          break;
        case 1:
          delay(200);
          analogWrite(RIGHT_MOTOR_PWM, 255);
          Serial.println("Movendo: Esquerda");
          break;
        case 2:
          delay(200);
          analogWrite(LEFT_MOTOR_PWM, 255);
          Serial.println("Movendo: Direita");
          break;
      }
    }

    if (movementInProgress && millis() - movementStartTime >= 2000) {
      analogWrite(LEFT_MOTOR_PWM, 0);
      analogWrite(RIGHT_MOTOR_PWM, 0);
      movementInProgress = false;
      currentCommandIndex++;
    }
  }
}
