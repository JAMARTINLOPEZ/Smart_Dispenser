#define IN1  16
#define IN2  17
#define IN3  21
#define IN4  22
#include <WiFi.h>
#include <PubSubClient.h>
const int irPin = 25;
int lastirState = LOW;
int irState = LOW;
#define NUMERO_PASTILLERO "Pastillero01"
#define TOPIC_RECEIVE "homeassistant/" NUMERO_PASTILLERO "/Receive"
#define TOPIC_SEND "homeassistant/" NUMERO_PASTILLERO "/Send"
const char* ssid = "UIDEE";
const char* password = "Domotica4$";
const char* mqttBroker = "172.16.0.99";
bool servoStopped = true;
WiFiClient espClient;
PubSubClient client(espClient);
// Secuencia de pasos (par máximo)
int paso [8][4] =
{
  {1, 0, 0, 0},
  {1, 1, 0, 0},
  {0, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 1},
  {0, 0, 0, 1},
  {1, 0, 0, 1}
};
 
void setup()
{
  Serial.begin(115200);
  // Todos los pines en modo salida
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(irPin, INPUT);
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
  if (String(topic) == TOPIC_RECEIVE) {
    Serial.print("Changing output to ");
    if(messageTemp == "true"){
    

    servoStopped = false;
  }
}
}


void reconnect() {
  // Loop until we're reconnected
  if (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(NUMERO_PASTILLERO, "raspi_dali_1", "admin")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe(TOPIC_RECEIVE);
      client.subscribe(TOPIC_SEND);
 
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

if (!servoStopped){
      for (int i = 0; i < 8; i++) {
                digitalWrite(IN1, paso[i][0]);
                digitalWrite(IN2, paso[i][1]);
                digitalWrite(IN3, paso[i][2]);
                digitalWrite(IN4, paso[i][3]);
                delay(10);
               }  
}

  // Detectar cambio de estado del botón
  if (irState != lastirState) {
    if (irState == LOW) { // Cambiar el estado del servo solo en la transición del botón de HIGH a LOW
      if (!servoStopped) {
            digitalWrite(IN1, LOW);
            digitalWrite(IN2, LOW);
            digitalWrite(IN3, LOW);
            digitalWrite(IN4, LOW);
        servoStopped = true;
        client.publish(TOPIC_SEND, "true");
      }
    }
  }      
    
  Serial.println(irState);
  //delay(100);
  lastirState = irState;
   if (WiFi.status() != WL_CONNECTED){
    setup_wifi();
    }
    if (!client.connected()) {
      reconnect();
    }
    client.loop();



}