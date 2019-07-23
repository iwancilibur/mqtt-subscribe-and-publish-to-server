#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// Update these with values suitable for your network.

const char* ssid = "iwancilibur";
const char* password = "12345678";
const char* mqtt_server = "192.227.204.234";
int led1=D5;
int led2=D6;
int led3=D7;

#define DHTTYPE DHT11
#define DHTPIN  D1
DHT dht(DHTPIN, DHTTYPE);

#define humidity_topic "sensor/humidity"
#define temperature_topic "sensor/temperature"
#define analog_topic "sensor/analog"

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(led1, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  dht.begin();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char *Xcontrol, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(Xcontrol);
  Serial.println("] ");
  if (strcmp(Xcontrol, "lampu1") == 0)
  { 
    for (int i=0;i<length;i++) {
    char receivedChar = (char)payload[i];
    if (receivedChar == '1')
       digitalWrite(led1, HIGH);
    if (receivedChar == '0')
       digitalWrite(led1, LOW);
    }
  }

  if (strcmp(Xcontrol, "lampu2") == 0)
  { 
    for (int i=0;i<length;i++) {
    char receivedChar = (char)payload[i];
    if (receivedChar == '1')
       digitalWrite(led2, HIGH);
    if (receivedChar == '0')
       digitalWrite(led2, LOW);
    }
  }

  if (strcmp(Xcontrol, "lampu3") == 0)
  { 
    for (int i=0;i<length;i++) {
    char receivedChar = (char)payload[i];
    if (receivedChar == '1')
       digitalWrite(led3, HIGH);
    if (receivedChar == '0')
       digitalWrite(led3, LOW);
    }
  }
  
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      client.subscribe("lampu1");
      client.subscribe("lampu2");
      client.subscribe("lampu3");
      // Once connected, publish an announcement...
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

bool checkBound(float newValue, float prevValue, float maxDiff) {
  return !isnan(newValue) &&
         (newValue < prevValue - maxDiff || newValue > prevValue + maxDiff);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  //client.publish("testiwan","suhu");
    client.loop();
    delay(1000);
    
    float temp = 0.0;
    float hum = 0.0;
    float diff = 1.0;
    int   analog=0;
    float newTemp = dht.readTemperature();
    float newHum = dht.readHumidity();
    int newAnalog=analogRead(A0);
    
    if (checkBound(newTemp, temp, diff)) {
      temp = newTemp;
      Serial.print("New temperature:");
      Serial.println(String(temp).c_str());
      client.publish(temperature_topic, String(temp).c_str(), true);
    }

    if (checkBound(newHum, hum, diff)) {
      hum = newHum;
      Serial.print("New humidity:");
      Serial.println(String(hum).c_str());
      client.publish(humidity_topic, String(hum).c_str(), true);
    }

    if (checkBound(newAnalog, analog, diff)) {
      analog = newAnalog;
      Serial.print("New Analog:");
      Serial.println(String(analog).c_str());
      client.publish(analog_topic, String(analog).c_str(), true);
    }
}
