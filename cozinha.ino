#include <TFT.h>
#include <SPI.h>

#define buttonPin1 22 //button pin for the Cozinha
#define buttonPin2 34 //button pin for the Piezzo
#define piezzoPin 27 //pin for the Piezzo

//pin declaration for the TFT
#define cs 23
#define dc 25
#define rst 26

TFT TFTscreen = TFT(cs, dc, rst);

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

  TFTscreen.background(204, 51, 77);
  TFTscreen.stroke(51, 178, 204);
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
        //ou seja, liga 2 primeiros LEDs do NeoPixel; Som de início == fur elise
        if(detectPress){currentState = PREPARAR;}
      }

      case PREPARAR:{
        //FALTA METER O QUE O CLIENTE VAI RECEBER
        //ou seja, passar imagens no TFT; loading atraves dos NeoPixeis (barra de progresso); piezzo toca musicas
        //MÚSICAS==> Game of Thrones || Super Mario || Never Gonna Give You Up || Tetris || Merry Christmas 
        if(detectPress){currentState = PRONTO;}
      }

      case PRONTO:{
        //FALTA METER O QUE O CLIENTE VAI RECEBER
        //ou seja, Piezzo toca; NeoPixeis piscam; TFT mostra imagem de pronto
        if(detectPress){currentState = ESPERA;}
      }
  }
}
