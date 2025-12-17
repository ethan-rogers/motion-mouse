#include "Mouse.h"
#include "Wire.h" 

// input

#define LEFT_CLICK 5
#define RIGHT_CLICK 9

#define X_OUT A0
#define Y_OUT A1
#define Z_OUT A2

// output
#define LEFT_LED 30
#define RIGHT_LED 17

const int MPU_ADDR = 0x68;

int16_t accelerometer_x, accelerometer_y, accelerometer_z; 
int16_t gyro_x, gyro_y, gyro_z;
int16_t temperature; 

// variables to get change in rotation
int xLast;
int yLast;
int zLast;

char tmp_str[7]; 

char* convert_int16_to_str(int16_t i) { 
  sprintf(tmp_str, "%6d", i);
  return tmp_str;
}

// calibration for the sensor
void setup() {
  Mouse.begin();
  Serial.begin(115200);


  // input pins
  pinMode(LEFT_CLICK, INPUT_PULLUP);
  pinMode(RIGHT_CLICK, INPUT_PULLUP);

  Wire.begin();
  Wire.beginTransmission(MPU_ADDR); 
  Wire.write(0x6B);
  Wire.write(0); 
  Wire.endTransmission(true);


  // output lights
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LEFT_LED, OUTPUT); 
  pinMode(RIGHT_LED, OUTPUT); 
}


void loop() {
  if (digitalRead(LEFT_CLICK) == LOW){
    if (!Mouse.isPressed(MOUSE_LEFT)) {
      Mouse.press(MOUSE_LEFT);
      Serial.println("LEFT CLICKED");
    }
    digitalWrite(LEFT_LED, LOW);
  }else
  {
    if (Mouse.isPressed(MOUSE_LEFT)) {
      Mouse.release(MOUSE_LEFT);
      Serial.println("LEFT RELEASED");
    }
    digitalWrite(LEFT_LED, HIGH);
  }


  if (digitalRead(RIGHT_CLICK) == LOW){
    if (!Mouse.isPressed(MOUSE_RIGHT)) {
      Mouse.press(MOUSE_RIGHT);
      Serial.println("RIGHT CLICKED");
    }
    digitalWrite(RIGHT_LED, LOW);
  }else
  {
    if (Mouse.isPressed(MOUSE_RIGHT)) {
      Mouse.release(MOUSE_RIGHT);
      Serial.println("RIGHT RELEASED");
    }
    digitalWrite(RIGHT_LED, HIGH);
  }

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); 
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 7*2, true); 
  

  accelerometer_x = Wire.read()<<8 | Wire.read();
  accelerometer_y = Wire.read()<<8 | Wire.read();
  accelerometer_z = Wire.read()<<8 | Wire.read(); 
  temperature = Wire.read()<<8 | Wire.read();
  gyro_x = Wire.read()<<8 | Wire.read(); 
  gyro_y = Wire.read()<<8 | Wire.read(); 
  gyro_z = Wire.read()<<8 | Wire.read();
  

  Serial.print("aX = "); Serial.print(convert_int16_to_str(accelerometer_x));
  Serial.print(" | aY = "); Serial.print(convert_int16_to_str(accelerometer_y));
  Serial.print(" | aZ = "); Serial.print(convert_int16_to_str(accelerometer_z));

  Serial.print(" | tmp = "); Serial.print(temperature/340.00+36.53);
  Serial.print(" | gX = "); Serial.print(convert_int16_to_str(gyro_x));
  Serial.print(" | gY = "); Serial.print(convert_int16_to_str(gyro_y));
  Serial.print(" | gZ = "); Serial.print(convert_int16_to_str(gyro_z));
  Serial.println();

}
