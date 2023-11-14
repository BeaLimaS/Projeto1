#define buttonPin 22 //button pin is 22

//variables for the State Machine
bool lastState = 0;  //last state of the terminal
bool currentState;     //current state of the terminal


void detectPress(){ //function to detect if the button is pressed
  bool currentState = digitalRead(buttonPin);
  if((lastState == LOW) && (currentState == HIGH)){
    buttonPressed = true;
  }
  else {
    buttonPressed = false;
  }
  lastState = currentState;
}

void 

void setup() {
  Serial.begin(9600);

  pinMode(buttonPin, INPUT); //BUTTON_PIN is an INPUT
}

void loop() {
  // put your main code here, to run repeatedly:

}
