#include <WiFi.h>
#include <WebServer.h>

WebServer server(80);

int contador = 0;

// ==========================
//     PÁGINA WEB
// ==========================
String paginaHTML() {
  String html = R"====(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
  /* Solo lo esencial para estilo tipo Bootstrap */
  body { font-family: Arial, sans-serif; text-align: center; padding: 20px; }
  #contador { font-size: 60px; margin-bottom: 20px; }
  button {
    font-size: 30px; padding: 10px 25px;
    margin: 5px; border-radius: 6px;
    border: 1px solid #007bff; background-color: #007bff; color: white;
    cursor: pointer;
  }
  button:active { background-color: #0069d9; }
  input[type=number] {
    font-size: 28px; width: 80px; text-align: center;
    margin: 5px; border-radius: 6px; border: 1px solid #ced4da;
  }
</style>

<script>
function sumar() { fetch('/sumar'); }
function restar() { fetch('/restar'); }
function reset() { fetch('/reset'); }
function setValor() {
  let valor = document.getElementById('inputValor').value;
  fetch('/set?valor=' + valor);
}

// Actualiza el contador cada 300ms
setInterval(() => {
  fetch('/valor')
    .then(response => response.text())
    .then(value => {
      document.getElementById('contador').innerText = value;
    });
}, 300);
</script>
</head>
<body>

<h1 id="contador">0</h1>

<button onclick="sumar()">SUMAR</button>
<button onclick="restar()">RESTAR</button>
<button onclick="reset()">RESET</button>
<br><br>
<input type="number" id="inputValor" placeholder="Valor">
<button onclick="setValor()">SET</button>

</body>
</html>
)====";
  return html;
}

// ==========================
//      RUTAS DEL SERVIDOR
// ==========================
void handleRoot() { server.send(200, "text/html", paginaHTML()); }

void handleSumar() { contador++; server.send(200, "text/plain", "ok"); }
void handleRestar() { contador--; server.send(200, "text/plain", "ok"); }
void handleReset() { contador = 0; server.send(200, "text/plain", "ok"); }
void handleSet() {
  if (server.hasArg("valor")) {
    contador = server.arg("valor").toInt();
  }
  server.send(200, "text/plain", "ok");
}
void handleValor() { server.send(200, "text/plain", String(contador)); }

// ==========================
//         SETUP
// ==========================
void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.softAP("ESP32-CONTADOR", "12345678");
  Serial.println("WiFi creada");
  Serial.print("IP del ESP32: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/sumar", handleSumar);
  server.on("/restar", handleRestar);
  server.on("/reset", handleReset);
  server.on("/set", handleSet);
  server.on("/valor", handleValor);

  server.begin();
  Serial.println("Servidor web iniciado");
}

// ==========================
//          LOOP
// ==========================
void loop() { server.handleClient(); }

