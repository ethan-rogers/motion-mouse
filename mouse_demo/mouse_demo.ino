#include "Mouse.h"
// MPU-6050 Accelerometer + Gyro

// Bluetooth module connected to digital pins 2,3
// I2C bus on A4, A5
// Servo on pin 0

#include <Wire.h>
//#include <SoftwareSerial.h>
#include <math.h>
#include <Servo.h>

#define MPU6050_I2C_ADDRESS 0x68

#define FREQ  30.0 // sample freq in Hz

#define LEFT_LED 30
#define RIGHT_LED 17

#define LEFT_CLICK 5
#define RIGHT_CLICK 9


// global angle, gyro derived
double gSensitivity = 65.5; // for 500 deg/s, check data sheet
double gx = 0, gy = 0, gz = 0;
double gyrX = 0, gyrY = 0, gyrZ = 0;
int16_t accX = 0, accY = 0, accZ = 0;

double gyrXoffs = -281.00, gyrYoffs = 18.00, gyrZoffs = -83.00;
double oldgx = 0, oldgy = 0, oldgz = 0;
double x_diff = 0, y_diff = 0, z_diff = 0;

double threshhold = 1;

void setup()
{      
  int error;
  uint8_t c;
  uint8_t sample_div;

  Serial.begin(115200);
  pinMode(13, OUTPUT); 
  //roll_servo.attach(9, 550, 2550);
  Wire.begin();

  i2c_write_reg (MPU6050_I2C_ADDRESS, 0x6b, 0x00);
  i2c_write_reg (MPU6050_I2C_ADDRESS, 0x1a, 0x01);
  i2c_write_reg(MPU6050_I2C_ADDRESS, 0x1b, 0x08);

  sample_div = 1000 / FREQ - 1;
  i2c_write_reg (MPU6050_I2C_ADDRESS, 0x19, sample_div);


  digitalWrite(13, HIGH);
  calibrate();
  digitalWrite(13, LOW);

}

void loop()
{
  int error;
  double dT;
  double ax, ay, az;
  unsigned long start_time, end_time;

  start_time = millis();


  
  read_sensor_data();

  // angles based on accelerometer
  ay = atan2(accX, sqrt( pow(accY, 2) + pow(accZ, 2))) * 180 / M_PI;
  ax = atan2(accY, sqrt( pow(accX, 2) + pow(accZ, 2))) * 180 / M_PI;

  // angles based on gyro (deg/s)
  gx = gx + gyrX / FREQ;
  gy = gy - gyrY / FREQ;
  gz = gz + gyrZ / FREQ;

  // complementary filter
  // tau = DT*(A)/(1-A)
  // = 0.48sec
  gx = gx * 0.96 + ax * 0.04;
  gy = gy * 0.96 + ay * 0.04;

  x_diff = (oldgx - gx);
  y_diff = (oldgy - gy);
  z_diff = (oldgz - gz);

  x_diff = (abs(x_diff) >= threshhold) ? x_diff : 0;
  y_diff = (abs(y_diff) >= threshhold) ? y_diff : 0;
  z_diff = (abs(z_diff) >= threshhold) ? z_diff : 0;

  digitalWrite(13, HIGH);
  Serial.print(x_diff, 2);
  Serial.print(", ");
  Serial.print(y_diff, 2);
  Serial.print(", ");
  Serial.println(z_diff, 2);
  digitalWrite(13, LOW);


  oldgx = gx;
  oldgy = gy;
  oldgz = gz;
    

  


    end_time = millis();
    if (digitalRead(LEFT_CLICK) == LOW){
      if (!Mouse.isPressed(MOUSE_LEFT)) {
        //Mouse.press(MOUSE_LEFT);
        //Serial.println("LEFT CLICKED");
      }
      Serial.println("LEFT CLICKED");
      digitalWrite(LEFT_LED, LOW);
    }else
    {
      if (Mouse.isPressed(MOUSE_LEFT)) {
        //Mouse.release(MOUSE_LEFT);
       // Serial.println("LEFT RELEASED");
      }
      Serial.println("LEFT RELEASED");
      digitalWrite(LEFT_LED, HIGH);
    }


    if (digitalRead(RIGHT_CLICK) == LOW){
      if (!Mouse.isPressed(MOUSE_RIGHT)) {
        //Mouse.press(MOUSE_RIGHT);
        //Serial.println("RIGHT CLICKED");
      }
      //Serial.println("RIGHT CLICKED");
      digitalWrite(RIGHT_LED, LOW);
    }else
    {
      //Serial.println("RIGHT RELEASED");
      if (Mouse.isPressed(MOUSE_RIGHT)) {
        //Mouse.release(MOUSE_RIGHT);
        //Serial.println("RIGHT RELEASED");
      }
      digitalWrite(RIGHT_LED, HIGH);
    }
  

}


void calibrate(){

  int x;
  long xSum = 0, ySum = 0, zSum = 0;
  uint8_t i2cData[6]; 
  int num = 500;
  uint8_t error;

  for (x = 0; x < num; x++){

    error = i2c_read(MPU6050_I2C_ADDRESS, 0x43, i2cData, 6);
    if(error!=0)
    return;

    xSum += ((i2cData[0] << 8) | i2cData[1]);
    ySum += ((i2cData[2] << 8) | i2cData[3]);
    zSum += ((i2cData[4] << 8) | i2cData[5]);
  }
  gyrXoffs = xSum / num;
  gyrYoffs = ySum / num;
  gyrZoffs = zSum / num;

  Serial.println("Calibration result:");
  Serial.print(gyrXoffs);
  Serial.print(", ");
  Serial.print(gyrYoffs);
  Serial.print(", ");
  Serial.println(gyrZoffs);
  
} 

void read_sensor_data(){
 uint8_t i2cData[14];
 uint8_t error;
 // read imu data
 error = i2c_read(MPU6050_I2C_ADDRESS, 0x3b, i2cData, 14);
 if(error!=0)
 return;


 accX = ((i2cData[0] << 8) | i2cData[1]);
 accY = ((i2cData[2] << 8) | i2cData[3]);
 accZ = ((i2cData[4] << 8) | i2cData[5]);

 gyrX = (((i2cData[8] << 8) | i2cData[9]) - gyrXoffs) / gSensitivity;
 gyrY = (((i2cData[10] << 8) | i2cData[11]) - gyrYoffs) / gSensitivity;
 gyrZ = (((i2cData[12] << 8) | i2cData[13]) - gyrZoffs) / gSensitivity;
 
}

// ---- I2C routines

int i2c_read(int addr, int start, uint8_t *buffer, int size)
{
  int i, n, error;

  Wire.beginTransmission(addr);
  n = Wire.write(start);
  if (n != 1)
  return (-10);

  n = Wire.endTransmission(false);    
  if (n != 0)
  return (n);

  Wire.requestFrom(addr, size, true);
  i = 0;
  while(Wire.available() && i<size)
  {
    buffer[i++]=Wire.read();
  }
  if ( i != size)
  return (-11);

  return (0);  
}


int i2c_write(int addr, int start, const uint8_t *pData, int size)
{
  int n, error;

  Wire.beginTransmission(addr);
  n = Wire.write(start);      
  if (n != 1)
  return (-20);

  n = Wire.write(pData, size);  
  if (n != size)
  return (-21);

  error = Wire.endTransmission(true); 
  if (error != 0)
  return (error);

  return (0);       
}


int i2c_write_reg(int addr, int reg, uint8_t data)
{
  int error;
  
  error = i2c_write(addr, reg, &data, 1);
  return (error);
}