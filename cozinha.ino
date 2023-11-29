#include <Wire.h>
#include <wifi.h>
#include <Bounce2.h>

#define buttonPin 22

enum States {
  ESPERA,
  ACEITE,
  PREPARAR,
  PRONTO
};

Bounce debouncer = Bounce();  // Cria um objeto Bounce para debouncing
States currentState = ESPERA;

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
  debouncer.attach(buttonPin);
  debouncer.interval(50);  // Tempo de debounce em milissegundos
}

void loop() {
  // Atualiza o estado do botão
  debouncer.update();

  // Verifica se o botão foi pressionado
  if (debouncer.fell()) {
    nextState();  // Chama a função para transição de estado
  }

  switch (currentState) {
    case ESPERA:
      sendCommandToSlave("ESPERA");
      delay(1000);
      currentState = ACEITE;
      break;

    case ACEITE:
      sendCommandToSlave("ACEITE");
      delay(1000);
      currentState = PREPARAR;
      break;

    case PREPARAR:
      sendCommandToSlave("PREPARAR");
      delay(1000);
      currentState = PRONTO;
      break;

    case PRONTO:
      sendCommandToSlave("PRONTO");
      delay(1000);
      currentState = ESPERA;  // Volta ao estado inicial para reiniciar o ciclo
      break;

    default:
      // Tratamento de erro, se necessário
      break;
  }

  // Seu código aqui
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
