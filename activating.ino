Servo servo;
#define SERVO_PIN 15 //d8

void servo_init() {
  servo.attach(SERVO_PIN);
  servo.write(0);
}

void actuating_start() {
  StaticJsonDocument<100> command;
  deserializeJson(command, sub_msg);
  Serial.println("Changing output to ");

  const char* command_value = command["valve_status"];
  Serial.print("command: ");
  Serial.println(command_value);
  actuator_status = "lock";
  servo.write(180);
}
