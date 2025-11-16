#include "Mouse.h"

// input

#define LEFT_CLICK 5
#define RIGHT_CLICK 9

#define X_OUT A0
#define Y_OUT A1
#define Z_OUT A2

// output
#define LEFT_LED 30
#define RIGHT_LED 17

// variables to get change in rotation
int xLast;
int yLast;
int zLast;

// calibration for the sensor
void setup() {
  Mouse.begin();
  Serial.begin(115200);


  // input pins
  pinMode(LEFT_CLICK, INPUT_PULLUP);
  pinMode(RIGHT_CLICK, INPUT_PULLUP);

  pinMode(X_OUT, INPUT);
  pinMode(Y_OUT, INPUT);
  pinMode(Z_OUT, INPUT);


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

// Read the raw analog values from the sensor pins
  int xValue = analogRead(X_OUT);
  int yValue = analogRead(Y_OUT);
  int zValue = analogRead(Z_OUT);


  int minVal = 1;
  int xMove = 0;
  int yMove = 0;

  int moveMultiplier = 5;

  if ((yValue - yLast) > minVal || (yValue - yLast)*-1 > minVal){
    xMove = (yValue - yLast)*moveMultiplier;
  }

  if ((xValue - xLast) > minVal || (yValue - yLast)*-1 > minVal){
    yMove = -(xValue - xLast)*moveMultiplier;
  }

  Mouse.move(xMove, yMove);

  xLast = xValue;
  yLast = yValue;
  zLast = zValue;
  // Print the values to the Serial Monitor
  //Serial.print("X: ");
  //Serial.print(xValue);
  //Serial.print(" | Y: ");
  //Serial.print(yValue);
  //Serial.print(" | Z: ");
  //Serial.println(zValue);

  // Small delay for stable readings
  //delay(100);

}
