#include <Wire.h>
#include <WiFi.h>

#define buttonPin 22

const char *ssid = "ESP32";
const char *password = "pass";
const uint16_t port = 56789;

WiFiClient client;

enum State {
  ESPERA,
  ACEITE,
  PREPARAR,
  PRONTO
};

State currentState = ESPERA;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;  //debounce time in milliseconds

void sendCommand() {
  if (client.connected()) {
    client.print(static_cast<int>(currentState));
    Serial.print("Sent command to client: ");
    Serial.println(currentState);
  }
}

void setup() {
  Serial.begin(115200);
  delay(10);

  // Connect to WiFi
   // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}


void loop() {

  int buttonState = digitalRead(buttonPin);

  if (buttonState == LOW && millis() - lastDebounceTime > debounceDelay) {
    currentState = static_cast<State>((currentState + 1) % 4); // Cycle through four states
    sendCommand();
    lastDebounceTime = millis();
  }

  // Try to connect to the client
  if (!client.connected()) {
    client.connect("esp32-client", port);
    Serial.println("Connected to client");
  }

/*  switch (currentState) {
    case ESPERA:
      enviarComando("ESPERA");
      currentState = ACEITE;
      break;

    case ACEITE:
      enviarComando("ACEITE");
      currentState = PREPARAR;
      break;

    case PREPARAR:
      enviarComando("PREPARAR");
      currentState = PRONTO;
      break;

    case PRONTO:
      enviarComando("PRONTO");
      currentState = ESPERA;  // Volta ao estado inicial para reiniciar o ciclo
      break;

    default:
      // Tratamento de erro, se necessário
      break;
  }
  lastTransitionTime = currentTime;*/
}
