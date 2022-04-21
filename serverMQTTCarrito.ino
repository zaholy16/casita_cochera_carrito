//Libreria para podernos conectar el dispositivo a Internet

//por Wi-Fi
#include <ESP8266WiFi.h>

//Libreria para podernos conectar al broker MQTT

#include <PubSubClient.h>

//Constantes para conectarnos a la red Wi-Fi mediante ssid y su contraseña

const char* ssid = "zaholy";
const char* password = "pelusatony";

//Dirección del broker MQTT

const char* mqtt_server = "test.mosquitto.org";

//Aquí se realiza configura la conexión del dispositivo para conectarse
//al Wi-Fi y al broker

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;

#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];

int value = 0;
const int AIA = 12;
const int AIB = 13;
const int BIA = 5;
const int BIB = 4;
byte velocidad = 250;

void setup_wifi() {

  delay(10);

  // Empezamos por conectarnos a una red WiFi
  Serial.println();
  Serial.print("Conectado a ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

  }

  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());

}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Llegó el mensaje del tema [");
  Serial.print(topic);
  Serial.print("] ");

  String message;

  for (int i = 0; i < length; i++) {
    message = message + (char) payload[i];  // convert *byte to string
    Serial.print((char)payload[i]);

  }

  Serial.println();

  Serial.print(message);

  //Acciones del carrito
  if(message=="derecha")
  {
    derecha(); 
    Serial.println("...GIRANDO A LA DERECHA...");
  }

  if(message=="izquierda")
  {
    izquierda();
    Serial.println("...GIRANDO A LA IZQUIERDA");
  }

  if(message=="avanzar")
  {
    avanzar();
    Serial.println("...AVANZANDO...");
  }

  if(message=="retroceder")
  {
    retroceder();
    Serial.println("...RETROCEDIENDO...");
  }

  if(message=="detenerse")
  {
    detenerse();
    delay(1000);
    Serial.println("...DETENIDO...");
  }

}

void reconnect() {

  // Bucle hasta que nos volvamos a conectar

  while (!client.connected()) {

    Serial.print("Intentando la conexión MQTT...");

    // Crea una identificación de cliente aleatoria

    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    // Intenta conectarse

    if (client.connect(clientId.c_str())) {
      Serial.println("conectado");

      // ... y se suscribe
      client.subscribe("mzmd/accionesMez");

    } else {

      Serial.print("fallido, rc =");
      Serial.print(client.state());
      Serial.println(" inténtalo de nuevo en 5 segundos");

      // Espere 5 segundos antes de volver a intentarlo

      delay(5000);

    }
  }
}

void setup() {

  pinMode(AIA, OUTPUT);
  pinMode(AIB, OUTPUT);
  pinMode(BIA, OUTPUT);
  pinMode(BIB, OUTPUT);

  pinMode(BUILTIN_LED, OUTPUT);     // Inicialice el pin BUILTIN_LED como salida
  
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

}

void izquierda()
{
  analogWrite(AIB, 0);
  analogWrite(AIA, 180);
  analogWrite(BIA, 0);
  analogWrite(BIB, 180);
}

void derecha()
{
  analogWrite(AIB, 180);
  analogWrite(AIA, 0);
  analogWrite(BIA, 180);
  analogWrite(BIB, 0);
}

void retroceder()
{
  analogWrite(AIA, 0);
  analogWrite(AIB, velocidad);
  analogWrite(BIA, 0);
  analogWrite(BIB, velocidad);
}

void avanzar()
{
  analogWrite(AIA, velocidad);
  analogWrite(AIB, 0);
  analogWrite(BIA, velocidad);
  analogWrite(BIB, 0);
}

void detenerse()
{
  analogWrite(AIA, 0);
  analogWrite(AIB, 0);
  analogWrite(BIA, 0);
  analogWrite(BIB, 0);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  
  client.loop();
}

 
