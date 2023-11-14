#define buttonPin 22 //button pin is 22

//variables for the State Machine
bool lastState = 0;  //last state of the terminal

enum States{ACEITA, PREPARAR, PRONTO, ESPERA};
States currentState = ESPERA;
bool buttonPressed = 0;

void detectPress(){ //function to detect if the button is pressed
  bool currentState = digitalRead(buttonPin);
  if((lastState == LOW) && (currentState == HIGH)){
    buttonPressed = 1;
  }
  else {
    buttonPressed = 0;
  }
  lastState = currentState;
}

void setup() {
  Serial.begin(9600);

  pinMode(buttonPin, INPUT); //BUTTON_PIN is an INPUT
}

void loop() {
  switch (currentState){
      case ESPERA:{
        //FALTA METER O QUE O CLIENTE VAI RECEBER
        //ou seja, vai desligar tudo o que estava ligado (entra em modo de espera)
        if(detectPress){currentState = ACEITA;}
      }

      case ACEITA:{
        //FALTA METER O QUE O CLIENTE VAI RECEBER
        //ou seja, liga 2 primeiros LEDs do NeoPixel; Som de início
        if(detectPress){currentState = PREPARAR;}
      }

      case PREPARAR:{
        //FALTA METER O QUE O CLIENTE VAI RECEBER
        //ou seja, passar imagens no TFT; loading atraves dos NeoPixeis
        if(detectPress){currentState = PRONTO;}
      }

      case PRONTO:{
        //FALTA METER O QUE O CLIENTE VAI RECEBER
        //ou seja, Piezzo toca; NeoPixeis piscam; TFT mostra imagem de pronto
        if(detectPress){currentState = ESPERA;}
      }
  }
}
