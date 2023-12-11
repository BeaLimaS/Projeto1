#include <WiFi.h>
#include <HTTPClient.h>
/*
const char* ssid = "Cozinha";
const char* password =  "123456789";
String serverName = "http://198.168.1.100";

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

 
void setup() {
  Serial.begin(9600);
 
  WiFi.begin(ssid, password);
   
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

}
 
void loop() { 
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;

      String serverPath = serverName + "/state?a=1";
      
      http.begin(serverPath.c_str());
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      http.end();
  }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}*/

#define buttonPin 27

const char *ssid = "Bea Lima";
const char *password = "123456789";

WiFiClient client;

enum State {
  ESPERA,
  ACEITE,
  PREPARAR,
  PRONTO
};

State currentState = ESPERA;

int buttonState = 0;      // Estado atual do botão
int lastButtonState = 0;  // Estado anterior do botão

IPAddress server_ip(192, 168, 45, 247);


void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);

  // Connect to WiFi
  int status = WiFi.begin("Bea Lima", "123456789");
  WiFi.mode(WIFI_STA);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting");
    delay(1000);
  }

  ESP_LOGI("MODUYGHNMJU", "Connected to wifi");
  ESP_LOGI("MODUYGHNMJU", "\nStarting connection...");
  // if you get a connection, report back via serial:
}


void loop() {
  if (!client.connect(server_ip, 80)) {
    ESP_LOGI("MODUYGHNMJU", "Client is not connected");
    delay(50);
    return;
  }
  ESP_LOGI("MODUYGHNMJU", "Connected");


  buttonState = !digitalRead(buttonPin);

  if (buttonState != lastButtonState) {
    if (buttonState == true) {
      Serial.println("dgrr");
      switch (currentState) {
        case ESPERA:
          client.println("ESPERA");
          ESP_LOGI("MODUYGHNMJU", "ESPERA");
          currentState = ACEITE;
          break;

        case ACEITE:
          client.println("ACEITE");
          ESP_LOGI("MODUYGHNMJU", "ACEITE");

          currentState = PREPARAR;
          break;

        case PREPARAR:
          client.println("PREPARAR");
          ESP_LOGI("MODUYGHNMJU", "PREPARAR");
          currentState = PRONTO;
          break;

        case PRONTO:
          client.println("PRONTO");
          ESP_LOGI("MODUYGHNMJU", "PRONTO");
          currentState = ESPERA;  // Volta ao estado inicial para reiniciar o ciclo
          break;

        default:
          // Tratamento de erro, se necessário
          break;
      }
    }
    lastButtonState = buttonState;
  }
}
