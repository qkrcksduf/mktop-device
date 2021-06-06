#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <Servo.h>
#include "variable.h"
#include "conf.h"

//WiFiClient espClient;
WiFiClientSecure espClient;
PubSubClient client(espClient);

char sub_msg[128];
char pub_msg[128];
String ip_address;
String device_id;
String actuator_status = "unlock";
bool callback_checker = false;
int temperature = 0;
int caution_temperature = 0;
int count = 0;

void callback(char* topic, byte* payload, unsigned int length);
void reconnect(void);
void device_init();
void device_serial_send();
void device_info_get();
void device_state_send();
void actuating_start();

void setup() {
  Serial.begin(9600);
  dht_start();
  servo_init();
  delay(100);

  setup_wifi();
  espClient.setInsecure();
  client.setServer(MQTT_SERVER, PORT);
  client.setCallback(callback);
  

  comPortBegin(); //시리얼 통신 시작
  device_init();
}

void loop() {
  client.subscribe(get_locking_topic().c_str());
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (device_risk_detection() || count == 10) {
    device_state_send();
    count = 0;
  }

  count++;
  //device_state_send();
  delay(100);
  client.loop();
}

void device_init() {
  Serial.println("device_init");
  Serial.println();

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  client.subscribe(get_device_info_sub_topic().c_str());
  device_serial_send();
  device_info_get();

  client.unsubscribe(get_device_info_sub_topic().c_str());
}
void device_info_get() {
  while (!callback_checker) {
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
  }
  callback_checker = false;

  StaticJsonDocument<100> device_info;
  DeserializationError error = deserializeJson(device_info, sub_msg);
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }
  device_id = device_info["id"].as<String>();
  Serial.print("device_id: ");
  Serial.println(device_id);

  caution_temperature = device_info["caution_temperature"];
  Serial.print("caution_temperature: ");
  Serial.println(caution_temperature);
}

void device_serial_send() {
  Serial.println("device_serial_send");
  StaticJsonDocument<30> device_serial;
  device_serial["serial"] = DEVICE_SERIAL;
  serializeJson(device_serial, Serial);
  serializeJson(device_serial, pub_msg);
  Serial.println();


  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  client.publish(get_device_serial_pub_topic().c_str(), pub_msg);
}

void callback(char* topic, byte* payload, unsigned int length) {
  callback_checker = true;
  Serial.print("Message arrived: ");
  Serial.println(topic);
  Serial.print("Message: ");
  String message;

  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    message += (char) payload[i];
  }
  Serial.println(" 입니다.");

  memcpy(sub_msg, payload, length);
  sub_msg[length] = '\0';
  Serial.println();

  if (String(topic) == get_locking_topic().c_str()) {
    struct Status status;

    delay(1000);
    lockControl(0x03);
    delay(5000);
    //    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266 Client", MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("connected");

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      delay(2000);
    }
  }
}
