#include "MFRC522.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define RST_PIN 4 // RST-PIN for RC522 - RFID - SPI - Modul GPIO5
#define SS_PIN  5 // SDA-PIN for RC522 - RFID - SPI - Modul GPIO4

const char* ssid     = "iwalel";
const char* password = "Emmaschatz1";
const char* mqtt_server = "192.168.0.23";

const char* pub_topic = "lisa/iot/donations/reader/1/donate";


MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance
WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi(){
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);    // Initialize serial communications
  SPI.begin();           // Init SPI bus
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  Serial.print("Starting...");
  mfrc522.PCD_Init();    // Init MFRC522
}

unsigned long  assemble_uid(byte *buffer, byte bufferSize) {
    unsigned long uid=0;
    // Take lower 4 bytes only, even if longer
    if (bufferSize > 4)
      bufferSize = 4;

    for (byte i = 0; i < bufferSize; i++) {
        uid += buffer[i] << (i*8);
    }
    return uid;
  }


void loop() {
  unsigned long uid = 0;

  if(!client.connected()) {
    reconnect();
  }
  client.loop();

  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  mfrc522.PICC_HaltA();
  //uid = assemble_uid(mfrc522.uid.uidByte, mfrc522.uid.size);
  client.publish(pub_topic, (uint8_t*)mfrc522.uid.uidByte, mfrc522.uid.size);
  //client.publish(pub_topic, "haha");

}
