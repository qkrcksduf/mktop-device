void setup_wifi() {
  delay(10);
  WiFi.mode(WIFI_STA); 
  WiFi.begin(SSID, PASSWORD);
  Serial.println("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    for (int i = 0; i < 500; i++) {
      delay(1);
    }
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected to SSID : ");
  Serial.println(WiFi.SSID());
}
