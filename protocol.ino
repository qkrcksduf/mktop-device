#include "conf.h"

const uint8_t  GET_STATUS = 0x31;
const uint8_t  SET_TIME = 0x32;
const uint8_t  LOCK_CONTROL = 0x33;
const uint8_t  GET_VERSION = 0x34;
const uint8_t  RESET = 0x34;

uint8_t recvData[32] = { 0x00 };
uint8_t dataLength = 0;
uint8_t commandId = 0;

bool getStatus(struct Status* status) {  //디바이스의 상태를 가져오는 메서드
  uint8_t sendData[2] = { 0x00, 0x00 };  // Data를 null로 처리 2바이트라서 둘다 null처리
  sendPacket(2, GET_STATUS, sendData); //패킷 송신
  if (recvPacket(&dataLength, &commandId, recvData)) { //패킷 수신
    //구조체 안에 데이터 넣기
    status->setTime = (int)recvData[0] << 8 | recvData[1]; 
    status->remainTime = recvData[2] << 8 | recvData[3];
    status->lockStatus = recvData[6];
    status->fireStatus = recvData[7];
    status->battery = recvData[8];
    return true;
  } else {
    return error("GetStatus");
  }
}

bool setTime(int minutes) {
  uint8_t data[2] = { (uint8_t)(minutes >> 8 & 0xFF), (uint8_t)(minutes << 8 & 0xFF) };
  sendPacket(2, SET_TIME, data);
  if (recvPacket(&dataLength, &commandId, recvData)) {
    return true;
  } else {
    return error("SetTime");
  }
}

bool lockControl(int command) {
  uint8_t data[1] = { (uint8_t)command };
  sendPacket(1, LOCK_CONTROL, data);
  if (recvPacket(&dataLength, &commandId, recvData)) {
    return true;
  } else {
    return error("LockControl");
  }
}

bool getVersion(int* version) {
  uint8_t data[1] = { 0x00 };
  sendPacket(1, GET_VERSION, data);
  if (recvPacket(&dataLength, &commandId, recvData)) {
    *version = (int)recvData[0] << 8 | recvData[1];
  } else {
    return error("GetVersion");
  }
}

bool requestReset() {
  uint8_t data[1] = { 0x00 };
  sendPacket(1, RESET, data);
  if (recvPacket(&dataLength, &commandId, recvData)) {
    return true;
  } else {
    return error("Reset");
  }
}

bool error(String command) {
#ifdef __DEBUG__
  Serial.print(">> ");
  Serial.print(command);
  Serial.println(" Failed !!");
#endif
  return false;
}
