#define DXL_BUS_SERIAL1 1  //Dynamixel on Serial1(USART1)  <-OpenCM9.04
#define DXL_BUS_SERIAL2 2  //Dynamixel on Serial2(USART2)  <-LN101,BT210
#define DXL_BUS_SERIAL3 3  //Dynamixel on Serial3(USART3)  <-OpenCM 485EXP

#define FIRST_SERVO_ID 1
#define LAST_SERVO_ID 2
#define PIXELS_LENGTH 800
#define PIXELS_WIDTH 480
#define TOUCHSCREEN_LENGTH 150   // (in mm) TO CONFIRM
#define TOUCHSCREEN_WIDTH 110

#define GOAL_POSITION 30

Dynamixel Dxl(DXL_BUS_SERIAL1);

double xBallPosition; // (in mm)
double yBallPosition;

void setServosPositions(int angle1, int angle2){
  if(angle1 < -30 || angle1 > 30)
    return;
  if(angle2 < -30 || angle2 > 30)
    return;

  int pos1 = angle1 * 1024 / 300 + 0x00000200;
  int pos2 = angle2 * 1024 / 300 + 0x00000200;

  byte payload[4];
  payload[0] = (byte)(pos1 & 0x000000ff);
  payload[1] = (byte)((pos1 & 0x0000ff00)>>8);
  payload[2] = (byte)(pos2 & 0x000000ff);
  payload[3] = (byte)((pos2 & 0x0000ff00)>>8);

  setServos(payload);
}

void setServo1Position(int angle){
  if(angle < -30 || angle > 30)
    return;

  int pos = angle * 1024 / 300 + 0x00000200;

  byte payload[2];
  payload[0] = (byte)(pos & 0x000000ff);
  payload[1] = (byte)((pos & 0x0000ff00)>>8);
  
  setServo(1, payload); 
}

void setServo2Position(int angle){
  if(angle < -30 || angle > 30)
    return;

  int pos = angle * 1024 / 300 + 0x00000200;

  byte payload[2];
  payload[0] = (byte)(pos & 0x000000ff);
  payload[1] = (byte)((pos & 0x0000ff00)>>8);

  setServo(2, payload); 
}

void setServo(byte id, byte* buffer){
  Dxl.initPacket(BROADCAST_ID, INST_SYNC_WRITE);
  /* From now, insert byte data to packet without any index or data length*/
  Dxl.pushByte(GOAL_POSITION);
  Dxl.pushByte(2);
  //push individual data length per 1 dynamixel, goal position needs 2 bytes(1word)

  Dxl.pushByte(id);
  Dxl.pushByte(buffer[0]);
  Dxl.pushByte(buffer[1]);

  /* just transfer packet to dxl bus without any arguments*/
  Dxl.flushPacket();
}

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
  if (nCount != 4) {
    //SerialUSB.println("data is corrupt!");
    return; // data is corrupt
  }

  int xPositionDetected, yPositionDetected;
  xPositionDetected = (int)buffer[0];
  xPositionDetected <<= 8;
  xPositionDetected |= (int)buffer[1];
  yPositionDetected = (int)buffer[4];
  yPositionDetected <<= 8;
  yPositionDetected |= (int)buffer[5];

  updateBallPosition(xPositionDetected, yPositionDetected);
}

void updateBallPosition(int xPos, int yPos){
  xBallPosition = xPos / PIXELS_LENGTH * TOUCHSCREEN_LENGTH - TOUCHSCREEN_LENGTH / 2;
  yBallPosition = yPos / PIXELS_WIDTH * TOUCHSCREEN_WIDTH - TOUCHSCREEN_WIDTH / 2;
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