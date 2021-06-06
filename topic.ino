
String get_device_state_pub_topic() {
  return "/v1/device/" + device_id + "/status";
}

String get_locking_topic() {
  return "/v1/device/" + device_id + "/valve";
}

String get_device_serial_pub_topic() {
  return "/v1/device/serial";
}

String get_device_info_sub_topic() {
  return "/v1/device/" + DEVICE_SERIAL;
}
