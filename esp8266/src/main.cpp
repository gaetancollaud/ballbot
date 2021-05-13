#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <console.h>

const char *ssid = "marly-iot";
const char *password = "jpTMP12Wp1";
const char *mqtt_server = "192.168.1.12";
const int mqtt_pord = 1884;
const char *mqtt_user = "ballbot";
const char *mqtt_password = "4EQzcQi4e1WAXC6Oi7l7";
String baseStatusTopic = "ballbot/status/";
String baseCommandTopic = "ballbot/cmd/";

void sendMessage(String, String);

Console console(&sendMessage);
WiFiClient espClient;
PubSubClient client(espClient);
bool ledState = false;

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void switchLed()
{
  ledState = !ledState;
  digitalWrite(BUILTIN_LED, ledState ? HIGH : LOW);
}

void setup_wifi()
{

  delay(10);
  // We start by connecting to a WiFi network
  Serial.print("i Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("i WiFi connected");
  Serial.print("i IP address: ");
  Serial.println(WiFi.localIP());
}

void sendMessage(String item, String value)
{

  switchLed();
  Serial.print("i set ");
  Serial.print(item);
  Serial.print("=");
  Serial.println(value);
  String topic = baseStatusTopic + item;
  client.publish(topic.c_str(), value.c_str());
}

void callback(char *topic, byte *payload, unsigned int length)
{
  switchLed();

  String item = String(topic).substring(baseCommandTopic.length());

  Serial.print("v ");
  Serial.print(item);
  Serial.print(" ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("i Attempting MQTT connection with client ");
    // Create a random client ID
    String clientId = "ballbot-";
    clientId += String(random(0xffff), HEX);
    Serial.print(clientId);
    Serial.print("... ");
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      String subTopic = baseCommandTopic + "#";
      Serial.print("i Subscribing to ");
      Serial.println(subTopic);
      client.subscribe(subTopic.c_str());
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  pinMode(BUILTIN_LED, OUTPUT); // Initialize the BUILTIN_LED pin as an output

  console.init();

  setup_wifi();
  client.setServer(mqtt_server, mqtt_pord);
  client.setCallback(callback);
}

void loop()
{

  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
  console.loop();

  // unsigned long now = millis();
  // if (now - lastMsg > 2000)
  // {
  //   lastMsg = now;
  //   ++value;
  //   snprintf(msg, MSG_BUFFER_SIZE, "%ld", value);
  //   sendMessage("test", msg);
  // }
}