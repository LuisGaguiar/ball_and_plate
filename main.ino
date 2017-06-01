#define DXL_BUS_SERIAL1 1  //Dynamixel on Serial1(USART1)  <-OpenCM9.04
#define DXL_BUS_SERIAL2 2  //Dynamixel on Serial2(USART2)  <-LN101,BT210
#define DXL_BUS_SERIAL3 3  //Dynamixel on Serial3(USART3)  <-OpenCM 485EXP

#define FIRST_SERVO_ID 1
#define LAST_SERVO_ID 2

#define GOAL_POSITION 30

Dynamixel Dxl(DXL_BUS_SERIAL1);

int xBallPosition;
int yBallPosition;

void setServos(byte* buffer) {
  Dxl.initPacket(BROADCAST_ID, INST_SYNC_WRITE);
  /* From now, insert byte data to packet without any index or data length*/
  Dxl.pushByte(GOAL_POSITION);
  Dxl.pushByte(2);
  //push individual data length per 1 dynamixel, goal position needs 2 bytes(1word)

  byte i = 0;
  byte id = FIRST_SERVO_ID;
  while (id <= LAST_SERVO_ID) {
    Dxl.pushByte(id++);
    Dxl.pushByte(buffer[i++]);
    Dxl.pushByte(buffer[i++]);
  }

  /* just transfer packet to dxl bus without any arguments*/
  Dxl.flushPacket();
}

void usbInterrupt(byte* buffer, byte nCount){
  if (nCount != 25) {
    //SerialUSB.println("data is corrupt!");
    return; // data is corrupt
  }

  xBallPosition = (int)buffer[2];
  xBallPosition <<= 8;
  xBallPosition |= (int)buffer[3];
  yBallPosition = (int)buffer[4];
  yBallPosition <<= 8;
  yBallPosition |= (int)buffer[5];
}

void setup() {
  // put your setup code here, to run once:
  Dxl.begin(3);
  for (byte id = FIRST_SERVO_ID; id <= LAST_SERVO_ID; ++id)
    Dxl.jointMode(id); // jointMod() is to use position mode

  SerialUSB.attachInterrupt(usbInterrupt);
}

void loop() {
  // do nothing
}