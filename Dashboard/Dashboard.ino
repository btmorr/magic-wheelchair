#include <RGBmatrixPanel.h>
#include <gamma.h>

#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>

#include "images.h"
#include "buttons.h"

/*
  - LED attached from pin 13 to ground (on board)
  - button attached to pin 2 from +5V
  - 10K resistor attached to pin 2 from ground


  - each eye board requires 3 ground connections
*/

/* pin map

[EYES] OE (output enable)         9
[EYES] LAT (latch)               10
[EYES] CLK (clock)               11
[EYES] blue1 (upper) output      26
[EYES] blue2 (bottom) output     29

[EYES] row select A              A0
[EYES] row select B              A1
[EYES] row select C              A2
[EYES] row select D              A3

wheel                            A8
*/

#define CLK 11
#define OE   9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false);

#define MAIN_WHEEL   A8

#define CIRCLE  0
#define STAR    1
#define HEART   2
int eyeShape = CIRCLE;



const int window = 20;
int readIdx = 0;
int mainWheel = 0;
int mainWheelReadings[window];
int mainWheelTotal = 0;
int mainWheelAvg = 0;
const int mainWheelMax = 1023;
int x_position = 0;

int delayTime = 40;

uint8_t *ptr;

void setup() {
  pinMode(MAIN_WHEEL, INPUT);

  attachInterrupt(digitalPinToInterrupt(FAST_LOOK_LEFT_BUTTON), fllCallback, RISING);
  attachInterrupt(digitalPinToInterrupt(SLOW_LOOK_LEFT_BUTTON), sllCallback, RISING);
  attachInterrupt(digitalPinToInterrupt(FAST_LOOK_RIGHT_BUTTON), flrCallback, RISING);
  attachInterrupt(digitalPinToInterrupt(SLOW_LOOK_RIGHT_BUTTON), slrCallback, RISING);
  attachInterrupt(digitalPinToInterrupt(FAST_ROLL_EYES_BUTTON), freCallback, RISING);
  attachInterrupt(digitalPinToInterrupt(SLOW_ROLL_EYES_BUTTON), sreCallback, RISING);

  for (int i=0; i<window; i++) {
    mainWheelReadings[i] = 0;
  }

  ptr = matrix.backBuffer(); // Get address of matrix data

  // write eye background from images header
  memcpy_P(ptr, circle, sizeof(circle));

  Serial.begin(9600);
  matrix.begin();
}

void loop() {
  if (fllPressed())
  {
    eyeShape = CIRCLE;
  } else if (sllPressed()) {
    eyeShape = STAR;
  } else if (flrPressed()) {
    eyeShape = HEART;
  }


  mainWheel = analogRead(MAIN_WHEEL);
  mainWheelTotal = mainWheelTotal - mainWheelReadings[readIdx];
  mainWheelTotal = mainWheelTotal + mainWheel;
  mainWheelReadings[readIdx] = mainWheel;
  mainWheelAvg = mainWheelTotal / window;

  x_position = ((mainWheelAvg * 10) / mainWheelMax) + 11;
  delay(delayTime);

  readIdx = (readIdx + 1) % window;
}
