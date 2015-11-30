
#include<Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303.h>

//define the multiplexer
#define TCAADDR 0x70
#define LeftPort 0x0
#define RightPort 0x1

//Accelerometer data
int LeftAcX,LeftAcY,LeftAcZ,LeftCpX,LeftCpY,LeftCpZ;
int LeftAcXz1, LeftAcYz1, LeftAcZz1;

int RightAcX, RightAcY,RightAcZ, RightCpX, RightCpY, RightCpZ;
int RightAcXz1, RightAcYz1, RightAcZz1;

//filter cutoff
float alpha;

//FSRs and buttons
int LeftFSR1, LeftFSR2, LeftFSR3, LeftThumbButton;
int RightFSR1, RightFSR2, RightFSR3, RightThumbButton;

int rightPitch, rightRoll, leftPitch, leftRoll;
int rightHeading, leftHeading;

Adafruit_LSM303 lsm1, lsm2;


//Time
int t;

void tcaselect(uint8_t i) {
  if (i > 7) return;
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
}

void setup(){
  Wire.begin();
  //set up the i2c
  Serial.begin(115200);

  //Serial.println("Setting Up");
  tcaselect(LeftPort);
  
  if (!lsm1.begin())
  {
    Serial.println("Oops ... unable to initialize the LSM303. Check your wiring!");
    while (1);
  }

  tcaselect(RightPort);
  
  if (!lsm2.begin())
  {
    Serial.println("Oops ... unable to initialize the LSM303. Check your wiring!");
    while (1);
  }
  
  //Set The z-1 values to zero
  LeftAcXz1 = 0;
  LeftAcYz1 = 0;
  LeftAcZz1 = 0;
  RightAcXz1 = 0;
  RightAcYz1 = 0;
  RightAcZz1 = 0;

  //set all fsrs to 0
  RightFSR1 = 0;
  RightFSR2 = 0;
  RightFSR3 = 0;
  RightThumbButton = 0;
  
  LeftFSR1 = 0;
  LeftFSR2 = 0;
  LeftFSR3 = 0;
  LeftThumbButton = 0;

  //Set the T value in ms
  t = 10; //ms
  
  alpha = .03; //the filter cutoff

  //Serial.println("Setup Finished");
  
}


void loop(){
  //Read all simple analog and digital information

  LeftFSR1 = analogRead(A2);
  LeftFSR2 = analogRead(A3);
  //LeftFSR3 = analogRead(A5);
  LeftThumbButton = digitalRead(2);

  //Read Left I2C Data
  tcaselect(LeftPort);
  lsm1.read();
  LeftAcX = (int)lsm1.accelData.x;
  LeftAcY = (int)lsm1.accelData.y;
  LeftAcZ = (int)lsm1.accelData.z;
  LeftCpX = (int)lsm1.magData.x;
  LeftCpY = (int)lsm1.magData.y;
  LeftCpZ = (int)lsm1.magData.z;
  
  //Filter Left Accel Data
  LeftAcX = (int)(alpha*(float)(LeftAcX) + (1-alpha)*(float)LeftAcXz1); //lowpass filter
  LeftAcY = (int)(alpha*(float)(LeftAcY) + (1-alpha)*(float)LeftAcYz1);
  LeftAcZ = (int)(alpha*(float)(LeftAcZ) + (1-alpha)*(float)LeftAcZz1);
  
  LeftAcXz1 = LeftAcX;
  LeftAcYz1 = LeftAcY;
  LeftAcZz1 = LeftAcZ;
  
  //get left roll, pitch, and heading

  leftRoll = getRoll(LeftAcX, LeftAcY, LeftAcZ);
  leftPitch = getPitch(LeftAcX, LeftAcY, LeftAcZ);
  leftHeading = getHeading(LeftCpX, LeftCpY);
  
  Serial.print(leftRoll);
  Serial.print(F(" "));
  Serial.print(leftPitch);
  Serial.print(F(" "));
  Serial.print(leftHeading);
  Serial.print(F(" "));
  Serial.print(LeftFSR1);
  Serial.print(F(" "));
  Serial.print(LeftFSR2);
  Serial.print(F(" "));
  Serial.print(LeftFSR3);
  Serial.print(F(" "));
  Serial.print(LeftThumbButton);
  Serial.print(F(" "));

  leftRoll = 0;
  leftPitch = 0;
  leftHeading = 0;
  LeftFSR1 = 0;
  LeftFSR2 = 0;
  LeftFSR3 = 0;
  LeftThumbButton = 0;
  
  //Read Right Data
  RightFSR1 = analogRead(A1);
  RightFSR2 = analogRead(A0);
  //RightFSR3 = analogRead(A2);
  RightThumbButton = digitalRead(3);
  
  //Read Right
  tcaselect(RightPort);
  lsm2.read();
  //Read Right I2C data
  RightAcX = (int)lsm2.accelData.x;
  RightAcY = (int)lsm2.accelData.y;
  RightAcZ = (int)lsm2.accelData.z;
  RightCpX = (int)lsm2.magData.x;
  RightCpY = (int)lsm2.magData.y;
  RightCpZ = (int)lsm2.magData.z;

  //Filter Right Accel Data
  RightAcX = (int)(alpha*(float)(RightAcX) + (1-alpha)*(float)RightAcXz1); //lowpass filter
  RightAcY = (int)(alpha*(float)(RightAcY) + (1-alpha)*(float)RightAcYz1);
  RightAcZ = (int)(alpha*(float)(RightAcZ) + (1-alpha)*(float)RightAcZz1);
  
  RightAcXz1 = RightAcX;
  RightAcYz1 = RightAcY;
  RightAcZz1 = RightAcZ;

  //get right pitch, roll and heading

  rightRoll = getRoll(RightAcX, RightAcY, RightAcZ);
  rightPitch = getPitch(RightAcX, RightAcY, RightAcZ);
  rightHeading = getHeading(RightCpX, RightCpY);

  Serial.print(rightRoll);
  Serial.print(F(" "));
  Serial.print(rightPitch);
  Serial.print(F(" "));
  Serial.print(rightHeading);
  Serial.print(F(" "));
  Serial.print(RightFSR1);
  Serial.print(F(" "));
  Serial.print(RightFSR2);
  Serial.print(F(" "));
  Serial.print(RightFSR3);
  Serial.print(F(" "));
  Serial.print(RightThumbButton);
  Serial.print(F(" "));

  //end signal
  Serial.println();
  
  //wait time delay before repeating
  delay(t);
}

int getHeading(int magX, int magY)
{
  // see section 1.2 in app note AN3192
  int heading = round(180*atan2(magY, magX)/PI);  // assume pitch, roll are 0
  
  if (heading <0)
    heading += 360;

  return heading;
}

float getRoll(int accelX, int accelY, int accelZ)
{
  float roll = atan2(accelY,sqrt(pow(accelX,2) + pow(accelZ,2))); //convert radians into degrees
  roll = roll * (180.0/PI) ;
  return roll;
}

float getPitch(int accelX, int accelY, int accelZ)
{
   float pitch = atan2(accelX,sqrt(pow(accelY,2) + pow(accelZ,2))); 
   pitch = pitch * (180.0/PI);
   return pitch;
}


//float getTiltHeading(int magX, int magY, int magZ, int accelX, int accelY, int accelZ)
//{
//  // see appendix A in app note AN3192 
//  float pitch = asin(-accelX);
//  float roll = asin(accelY/cos(pitch));
//  
//  float xh = magX * cos(pitch) + magZ * sin(pitch);
//  float yh = magX * sin(roll) * sin(pitch) + magY * cos(roll) - magZ * sin(roll) * cos(pitch);
//  float zh = -magX * cos(roll) * sin(pitch) + magY * sin(roll) + magZ * cos(roll) * cos(pitch);
//
//  float heading = 180 * atan2(yh, xh)/PI;
//  if (yh >= 0)
//    return heading;
//  else
//    return (360 + heading);
//}

