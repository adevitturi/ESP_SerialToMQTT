#include <WiFi.h>
#include <PubSubClient.h>

#define LED            21

const char* ssid = "SSID";
const char* password = "PASSWORD";
const char* mqtt_Broker = "BROKER_IP";
const char* mqttClient = "CLIENT_NAME";
const char* mqttUser = "USR_NAME";
const char* mqttPassword = "PASS";                    
const char * topicPublish = "TOPIC_PUBLISH";

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output

  Serial.begin(19200);
  
  delay(1000);
  
  setup_wifi();
  client.setServer(mqtt_Broker, 1883);
  client.setCallback(callback);
  //client.subscribe(topicSubscribe1);
}

void setup_wifi() 
{
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

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(mqttClient, mqttUser, mqttPassword )) 
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("outTopic/sample1", "Conexion establecida");
      // ... and resubscribe
      //client.subscribe(topicSubscribe1);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

uint8_t incomingByte = 0;
long start = 0;
long endd = 0;
long periodd = 0;
bool f_serialrx = false;
unsigned int i = 0;
uint8_t msg_to_publish[100];

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (Serial.available() > 0) {
    if(!f_serialrx)
    {
      start = millis();
      f_serialrx = true;
      i = 0;
    }   
    
    // read the incoming byte:
    incomingByte = Serial.read();

    if(incomingByte != 10)
    {
      // say what you got:
      msg_to_publish[i] = incomingByte;
      i ++;
    }
    else
    {
      msg_to_publish[i] = 10;
      client.publish(topicPublish, msg_to_publish, i);
      endd = millis();
      periodd = endd - start;
      Serial.println("Msg sent!");
      Serial.println("Delay in transmition (ms):");
      Serial.println(periodd);
      f_serialrx = false;
    }
  }  
}
