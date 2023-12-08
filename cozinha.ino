#include <Wire.h>
#include <WiFi.h>

#define buttonPin 27

const char *ssid = "Cozinha";
const char *password = "123456789";
const uint16_t port = 80;

WiFiClient client;

enum State {
  ESPERA,
  ACEITE,
  PREPARAR,
  PRONTO
};

State currentState = ACEITE ;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;  //debounce time in milliseconds

void sendCommand() {
  if (client.connected()) {
    client.print(static_cast<int>(currentState));

    Serial.print("Sent command to client: ");
    Serial.println(currentState);
    delay(1000);
  }
}

IPAddress server_ip(192, 168, 1, 1);

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP);

  // Connect to WiFi
   int status = WiFi.begin(ssid, password);
  if ( status != WL_CONNECTED) {
    Serial.println("Couldn't get a wifi connection");
    // don't do anything else:
    while(true);
  }
  else {
    Serial.println("Connected to wifi");
    Serial.println("\nStarting connection...");
    // if you get a connection, report back via serial:
}
}


void loop() {
  if (!client.connect(server_ip , 80)) { return; }

  int buttonState = !digitalRead(buttonPin);

  // Try to connect to the client
 // if (!client.connected()) {
   // client.connect("esp32-client", port);
    //Serial.println("Not connected to client");
  //}
  Serial.println("VGJHVGJHKHUHB J");
  client.println("PREPARAR");

  switch (currentState) {
    case ESPERA:
      client.println("ESPERA");
      currentState = ACEITE;
      break;

    case ACEITE:
      client.println("ACEITE");
      currentState = PREPARAR;
      break;

    case PREPARAR:
      client.println("PREPARAR");
      currentState = PRONTO;
      break;

    case PRONTO:
      client.println("PRONTO");
      currentState = ESPERA;  // Volta ao estado inicial para reiniciar o ciclo
      break;

    default:
      // Tratamento de erro, se necessário
      break;
  }
}
