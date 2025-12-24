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
int16_t x_last = 0;
int16_t y_last = 0;
int16_t z_last = 0;

int16_t x_frames = 0;
int16_t y_frames = 0;
int16_t z_frames = 0;

int16_t filter = 100;
int16_t consecutive_franes = 4;

char first_frame = 1;

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

  if (!first_frame){
    // get difference
    int16_t x_diff = gyro_x - x_last;
    int16_t y_diff = gyro_y - y_last;
    int16_t z_diff = gyro_z - z_last;

    // filter by value
    x_diff = (x_diff < filter && x_diff > -filter) ? 0 : x_diff;
    y_diff = (y_diff < filter && y_diff > -filter) ? 0 : y_diff;
    z_diff = (z_diff < filter && z_diff > -filter) ? 0 : z_diff;

    // filter outliers
    x_frames = (x_diff) ? (x_frames + 1) : 0;
    y_frames = (y_diff) ? (y_frames + 1) : 0;
    z_frames = (z_diff) ? (z_frames + 1) : 0;

    x_diff = (x_frames > consecutive_franes) ? x_diff : 0;
    y_diff = (y_frames > consecutive_franes) ? y_diff : 0;
    z_diff = (z_frames > consecutive_franes) ? z_diff : 0;

    Serial.print("aX = "); Serial.print(convert_int16_to_str(x_diff));
    Serial.print(" | aY = "); Serial.print(convert_int16_to_str(y_diff));
    Serial.print(" | aZ = "); Serial.print(convert_int16_to_str(z_diff));

    Serial.println();
  }else{
    first_frame = 0;
  }

  x_last = gyro_x;
  y_last = gyro_y;
  z_last = gyro_z;


  



}
