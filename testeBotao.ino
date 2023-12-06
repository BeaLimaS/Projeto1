const int buttonPin = 27;  // Pino do botão
int buttonState = 0;      // Estado atual do botão
int lastButtonState = 0;  // Estado anterior do botão

enum State {
  STATE_1,
  STATE_2,
  STATE_3,
  STATE_4
};

State currentState = STATE_1;

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP);

  Serial.println("Pressione o botão para avançar para o próximo estado.");
}

void loop() {
  buttonState = !digitalRead(buttonPin);

  if (buttonState != lastButtonState) {
    // Se o estado do botão mudou
    if (buttonState == HIGH) {
      // Se o botão foi pressionado
      switch (currentState) {
        case STATE_1:
          Serial.println("Estado 1");
          currentState = STATE_2;
          break;
        case STATE_2:
          Serial.println("Estado 2");
          currentState = STATE_3;
          break;
        case STATE_3:
          Serial.println("Estado 3");
          currentState = STATE_4;
          break;
        case STATE_4:
          Serial.println("Estado 4");
          currentState = STATE_1;
          break;
      }
    }
    lastButtonState = buttonState;
  }

  // Aqui você pode adicionar a lógica específica para cada estado
  // ...

  delay(30);  // Pequeno atraso para evitar leituras rápidas do botão
}
