#include "DHT.h"

#define DHTPIN 0 //d3
#define DHTTYPE DHT22


DHT dht(DHTPIN, DHTTYPE);

void dht_start() {
  dht.begin();
}

boolean device_risk_detection() {
  if (dht.readTemperature() > caution_temperature) {
    return true;
  }
  return false;
}

void device_state_send() {
  Serial.println("device_state_send");
  Serial.println(dht.readTemperature());
  StaticJsonDocument<200> value;
  temperature = dht.readTemperature();
  value["temperature"] = temperature;
  value["valve_status"] = actuator_status;
  value["serial"] = DEVICE_SERIAL;
  value["device_id"] = device_id;
  serializeJson(value, Serial);
  serializeJson(value, pub_msg);
  Serial.println();

  Serial.print("topic: "); 
  Serial.println("get_device_state_pub_topic().c_str()");
  client.publish(get_device_state_pub_topic().c_str(), pub_msg);

}
