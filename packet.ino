#include "conf.h"
#include <SoftwareSerial.h>

SoftwareSerial commPort;

#define STX 0x02
#define ETX 0x03

void comPortBegin() {
  commPort.begin(9600, SWSERIAL_8N1, 13, 15);
}

void sendPacket(uint8_t dataLength, uint8_t id, uint8_t data[]) { 
  uint8_t packet[32]; //패킷의 길이, 부족할 경우 늘려도 됨, 
  int idx = 0; //인덱스

  // 패킷 구성 시작
  packet[idx++] = STX;  //시작을 확인하기 위한 명령
  packet[idx++] = dataLength + 1; //Command Id와 Data의 바이트 합(Length 0x03)
  packet[idx++] = id; // Command Id (0x31)

  for (int i = 0; i < dataLength; i++) //데이터 길이만큼 반복해서 패킷을 채워준다.
    packet[idx++] = data[i];

  packet[idx++] = getCheckSum(dataLength + 3, packet); //CRC (XOR 사용), dataLenth + 3 = 데이터 길이 + STX(1) + Length(1) + Command Id(1)  
  packet[idx++] = ETX; //끝을 확인하기 위한 명령

  for (int i = 0; i < idx; i++) { //시리얼통신을 통해 실제 디바이스로 패킷 전송
    commPort.write(packet[i]);
  }

  printPacket("Send >> ", idx, packet); //보내는 패킷 확인
}

//CRC 계산 - 계산 범위는 STX 부터 Data 까지의 XOR 값 이다.
uint8_t getCheckSum(uint8_t length, uint8_t packet[]) 
{
  uint8_t XOR = 0;
  for (int i = 0; i < length; i++) {
    XOR = XOR ^ packet[i];
  }
  return XOR;
}

bool recvPacket(uint8_t* dataLength, uint8_t* id, uint8_t data[]) {
  uint8_t packet[32];

  unsigned long startTime = millis();

  //1초간 응답이 읽히는 데이터가 없으면 다시 false 리턴
  while (commPort.available() <= 0) {
    if ((millis() - startTime) > 1000)
      return false;
    delay(100);
  }

  uint8_t inByte = commPort.read();  //read는 1바이트 단위로 읽는다.
  if (inByte != STX) {  // 첫 번째 명령이 STX가 아닌경우 
    commPort.flush();   // 버퍼 초기화
    return false;       // 제대로 된 명령이 아니기 때문에 false 리턴
  }

  int idx = 0;  
  packet[idx++] = inByte;  // 패킷에 STX 넣기
  uint8_t length = commPort.read(); //길이 읽어오기
  packet[idx++] = length; //Length 넣기

  for (int i = 0; i < length && i < 14; i++) // Command Id + 데이터 읽어오기
    packet[idx++] = commPort.read(); 

  uint8_t crc = getCheckSum(length + 2, packet); //CRC 계산
//  Serial.print("CRC = "); Serial.print(length); Serial.print(" "); Serial.println(crc);
  uint8_t recvCrc = commPort.read(); // CRC 읽어오기
  packet[idx++] = recvCrc;
  inByte = commPort.read();  //ETX 읽어오기
  packet[idx++] = inByte;
  
  printPacket("Recv << ", idx, packet); //읽어온 패킷 출력

  if (inByte != ETX || crc != recvCrc) { //ETX 다르던가 crc가 다르다면 잘못들어온 패킷이기 때문에 false를 리턴한다.
    commPort.flush(); //버퍼 비우기
    return false;
  }

  idx = 1; 
  *dataLength = packet[idx++] - 1; // Length - 1바이트(Command Id) = Data
  *id = packet[idx++]; //Command Id

  for (int i = 0; i < *dataLength; i++) {
    data[i] = packet[idx++];  //data 길이만큼 채워넣기
  }

  return true;
}

void printPacket(String dir, uint8_t length, uint8_t packet[]) { // 패킷의 구조 시리얼 모니터에 출력
  Serial.print(dir);
  for (int i = 0; i < length; i++) {
    Serial.print(packet[i], HEX);//16진수 형태로 출력
    Serial.print(" ");
  }
  Serial.println();
}
