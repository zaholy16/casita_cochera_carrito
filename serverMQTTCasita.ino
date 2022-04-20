// Libreria para podernos conectar el dispositivo por Wi-Fi
#include <ESP8266WiFi.h>

// Libreria para podernos conectar al broker MQTT
#include <PubSubClient.h>
#include "Servo.h"

// Constantes para conectarnos a la red Wi-Fi mediante ssid y su contraseña
const char *ssid = "zaholy";
const char *password = "pelusatony";

// Dirección del broker MQTT
const char *mqtt_server = "test.mosquitto.org";

// Configuración de conexión del dispositivo para conectarse al Wi-Fi y al broker
WiFiClient espClient;

PubSubClient client(espClient);

unsigned long lastMsg = 0;

#define MSG_BUFFER_SIZE (50)

char msg[MSG_BUFFER_SIZE];

int value = 0;
const int servo = 4;
const int servo2 = 14;
const int relay = 5;

Servo myservo;
int angulo = 0;
int paso = 180;
String comando;

Servo myservo2;
int angulo2 = 180;
int paso2 = 0;
String comando2;

void setup_wifi()
{

  delay(10);

  // Empezamos por conectarnos a una red WiFi
  Serial.println();
  Serial.print("Conectado a ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Llegó el mensaje del tema [");
  Serial.print(topic);
  Serial.print("] ");

  String message;

  for (int i = 0; i < length; i++)
  {
    message = message + (char)payload[i]; // convert *byte to string
    Serial.print((char)payload[i]);
  }

  Serial.println();

  Serial.print(message);

  if (message == "cerrar")
  {
    cerrar();
    Serial.println("...CERRANDO...");
  }

  if (message == "abrir")
  {
    abrir();
    Serial.println("...ABRIENDO...");
  }

  if (message == "encender")
  {
    encender();
    Serial.println("...ENCENDIDO...");
  }

  if (message == "apagar")
  {
    apagar();
    Serial.println("...APAGADO...");
  }
}

void reconnect()
{

  // Bucle hasta que nos volvamos a conectar

  while (!client.connected())
  {

    Serial.print("Intentando la conexión MQTT...");

    // Crea una identificación de cliente aleatoria
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    // Intenta conectarse
    if (client.connect(clientId.c_str()))
    {
      Serial.println("conectado");

      // ... y se suscribe
      client.subscribe("mzmd/accionesMez");
    }
    else
    {
      Serial.print("fallido, rc =");
      Serial.print(client.state());
      Serial.println(" inténtalo de nuevo en 5 segundos");

      // Espere 5 segundos antes de volver a intentarlo
      delay(5000);
    }
  }
}

void setup()
{

  myservo.attach(servo);
  myservo.write(0);
  delay(1000);
  myservo2.attach(servo2);
  myservo2.write(180);
  delay(1000);

  pinMode(relay, OUTPUT); // Inicialice el pin del relay como salida

  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void cerrar()
{
  
  if (angulo <= (180 - paso))
  {
    angulo += paso;

    for (int i = 0; i <= angulo; i++)
    {
      myservo.write(i);
      myservo2.write(180-i);
      delay(25);
    }
  }
/*
  if (angulo2 >= (0 + paso2))
  {
    angulo2 -= paso2;
    for (int i = angulo2; i >= 0; i--)
    {
      myservo2.write(i);
      delay(25);
    }
  }*/
  
}

void abrir()
{
  
  if (angulo >= (0 + paso))
  {
    angulo -= paso;
    for (int i = 180; i >= angulo; i--)
    {
      myservo.write(i);
      myservo2.write((i-180)* (-1));
      delay(25);
    }
  }
/*
  if (angulo2 <= (180 - paso2))
  {
    angulo2 += paso2
    for (int i = 0; i <= angulo2; i++)
    {
      myservo2.write(i);
      delay(25);
    }
  }*/
}

void encender()
{
  digitalWrite(relay, LOW);
}

void apagar()
{
  digitalWrite(relay, HIGH);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }

  client.loop();
}
