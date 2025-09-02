#include <WiFi.h>
#include <WebServer.h>
#include <vector>

const char* ssid = "Your_SSID";
const char* password = "Your_Password";
std::vector<char> CommandList;

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

  // Rotas
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
  CommandList.pushback('F');
  Serial.println("Comando: Frente");
  server.send(200, "text/html", getPage());
}

void handleLeft() {
  CommandList.pushback('L');
  Serial.println("Comando: Esquerda");
  server.send(200, "text/html", getPage());
}

void handleRight() {
  CommandList.pushback('R');
  Serial.println("Comando: Direita");
  server.send(200, "text/html", getPage());
}

void handleStart() {
  Serial.println("Comando: Start");
  Serial.println(CommandList);
  server.send(200, "text/html", getPage());
}

void handleUndo() {
  CommandList.popback();
  Serial.println("Comando: Undo (remover último)");
  server.send(200, "text/html", getPage());
}

void handleClear() {
  CommandList.clear();
  Serial.println("Comando: Clear (limpar fila)");
  server.send(200, "text/html", getPage());
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
