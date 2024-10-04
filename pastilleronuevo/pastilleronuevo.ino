#include <ESP32Servo.h>
#include <WiFi.h>
#include <PubSubClient.h>

Servo myservo;
const int irPin = 22;
int lastirState = LOW;
int irState = LOW;
bool servoStopped = false;
const int servoPin = 16;
#define NUMERO_SONDA "sonda04"
#define TOPIC_PASTILLERO "homeassistant/" NUMERO_SONDA "/Pastillero"


const char* ssid = "UIDEE";
const char* password = "Domotica4$";
const char* mqttBroker = "172.16.0.99";
WiFiClient espClient;
PubSubClient client(espClient);

void setup() 
{ 
  Serial.begin(115200);
  myservo.attach(servoPin);
  pinMode(irPin, INPUT); // Se usa INPUT_PULLUP para activar la resistencia interna de pull-up
  
  client.setServer(mqttBroker, 1883);
  client.setCallback(callback);
} 


void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  if (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  }
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
 
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
  Serial.print(messageTemp);
  if (String(topic) == TOPIC_PASTILLERO) {
    Serial.print("Changing output to ");
    if(messageTemp == "true"){
        myservo.attach(servoPin); // Vuelve a activar el servo
        myservo.write(120); // Reanudar en una posición específica (ajusta según tus necesidades)
        servoStopped = false;
        //tone(buzzerPin, 1000); // Send 1KHz sound signal...
    }

}
}



void reconnect() {
  // Loop until we're reconnected
  if (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(NUMERO_SONDA, "raspi_dali_1", "admin")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe(TOPIC_PASTILLERO);
 
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void loop() 
{ 
  irState = digitalRead(irPin);
  
  // Detectar cambio de estado del botón
  if (irState != lastirState) {
    if (irState == LOW) { // Cambiar el estado del servo solo en la transición del botón de HIGH a LOW
      if (!servoStopped) {
        myservo.detach(); // Desactiva el servo
        servoStopped = true;
      }
    }
  }
  
  lastirState = irState;
   if (WiFi.status() != WL_CONNECTED){
    setup_wifi();
    }
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
}