#include <PubSubClient.h>
#include <ESP8266WiFi.h>

#define WIFI_SSID "xxxxxxxxxx"  // SSID
#define WIFI_PASSWORD "xxxxxxxxxxx"
#define RELAY 0 // relay connected to  GPIO0

char mqttServer[]  = "xx.xx.xx.xx"; // MQTT server IP or network name
int mqttPort  = 1883; //1883 is the default port for MQTT. Change if necessary
char deviceId[]  = "vhodnavrata"; //every device should have a different name
char topic[]  = "cmnd/klet/predprostor/vhodna_vrata"; //the topic should be different for each device as well
char mqttUser[]  = "mqtt_username";
char mqttPassword[]  = "mqtt_password";

WiFiClient wifiClient;
PubSubClient client(wifiClient);
int status = WL_IDLE_STATUS;

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println("Starting");
  pinMode(RELAY,OUTPUT);
  digitalWrite(RELAY,HIGH);
}

void loop() {
  if ( !client.connected() ) {
    connect();
  }
  client.loop();
}

const String unlockstring = "UNLOCK";
byte close[] = {0xA0, 0x01, 0x01, 0xA2};
byte open[] = {0xA0, 0x01, 0x00, 0xA1};

void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message = message + (char)payload[i];
  }
  if(message == unlockstring) { 
    Serial.println("Performing unlock sequence");
    digitalWrite(RELAY,LOW);
    delay(1600);
    digitalWrite(RELAY,HIGH);
  }
}

void connect() {
  while (!client.connected()) {
    status = WiFi.status();
    if ( status != WL_CONNECTED) {
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.print("Connected to ");
      Serial.println(WIFI_SSID);
    }
    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);
    if (client.connect(deviceId, mqttUser, mqttPassword)) {
      client.subscribe(topic);
      Serial.println("Connected to MQTT Server");
    } else {
      Serial.print("[FAILED] [ rc = ");
      Serial.print(client.state() );
      Serial.println(" : retrying in 5 seconds]");
      delay(5000);
    }
  }
}
