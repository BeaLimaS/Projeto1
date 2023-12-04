#include <Wire.h>
#include <WiFi.h>

#define buttonPin 22

const char *ssid = "ESP32";
const char *password = "pass";
const int i2cSlaveAddress = 8;  // I2C SLAVE I2C address

enum States {
  ESPERA,
  ACEITE,
  PREPARAR,
  PRONTO
};


States currentState = ESPERA;
unsigned long lastTransitionTime = 0;
unsigned long debouncePeriod = 1000; //debounce time in milliseconds

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);

//initialize AP
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao Wi-Fi...");
  }
  Serial.println("Conectado ao Wi-Fi!");
  Serial.print("Endereço IP do Cliente: ");
  Serial.println(WiFi.localIP());
  }

// Inicialize I2C address
  Wire.begin(i2cSlaveAddress);
  Wire.onReceive(receberInstrucao);
}

void loop() {
  unsigned long currentTime = millis();

   // Adiciona um debounce simples
  if (tempoAtual - ultimoTempoTransicao >= periodoDebounce) {
  switch (currentState) {
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
  lastTransitionTime = currentTime;
  }
}

void sendCommandToSlave(String command) {
  Serial.print("[ENVIANDO COMANDO] ");
  Serial.println(command);
  Serial1.print(command);
  Serial1.print('\n');  // Adiciona uma nova linha para indicar o final do comando
  delay(100);  // Aguarda para evitar congestionamento na comunicação serial
}

void nextState() {
  // Lógica para transição de estado
  currentState = static_cast<States>((currentState + 1) % 4);
}
