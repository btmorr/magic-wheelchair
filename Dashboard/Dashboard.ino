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

fast look left button             2
slow look left button             3
[EYES] OE (output enable)         9
[EYES] LAT (latch)               10
[EYES] CLK (clock)               11
fast look right button           18
slow look right button           19
fast roll eyes button            20
slow roll eyes button            21
[EYES] blue1 (upper) output      26
[EYES] blue2 (bottom) output     29

[EYES] row select A              A0
[EYES] row select B              A1
[EYES] row select C              A2
[EYES] row select D              A3
*/

#define CLK 11
#define OE   9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false);

// varistor pin map
/*
 * Wheel behavior:
 * 
 * The main wheel should ___
 * The secondary wheel should ____
 */
#define MAIN_WHEEL   A8
//#define SEC_WHEEL    A1
/*
 * Blink slider behavior:
 * 
 * The blink slider should determine how open or closed the eyes appear to be. Also,
 * this should affect the frequency and speed of blinking. If the eyes are more open
 * blinking should be faster and less frequent. If they are mostly closed, blinking
 * should be slower but more frequent. Note also that if the eyes are partially
 * closed, the "roll eyes" function should only roll up to the top of the open
 * portion of the eye, not up behind the eyelid.
 */
#define BLINK_SLIDER A7`

// other pins
#define BOARD_LED_PIN 13

const int window = 20;
int readIdx = 0;

int mainWheel = 0;
int mainWheelReadings[window];
int mainWheelTotal = 0;
int mainWheelAvg = 0;
const int mainWheelMax = 1023;
int x_position = 0;

int blinkSlide = 0;
int blinkSlideReadings[window];
int blinkSlideTotal = 0;
int blinkSlideAvg = 0;
int blinkSlideMax = 1023;
int delayTime = 40;

uint8_t *ptr;

void ledOn()
{
  digitalWrite(BOARD_LED_PIN, HIGH);
}

void ledOff()
{
  digitalWrite(BOARD_LED_PIN, LOW);
}

/* draw a star with the top point at x,y */
void drawStar(int x, int y)
{
  int Ax = x-4;
  int Ay = y+12;
  int Bx = x;
  int By = y;
  int Cx = x+4;
  int Cy = y+12;
  int Dx = x-6;
  int Dy = y+6;
  int Ex = x+6;
  int Ey = y+6;
  //star lines
  matrix.drawLine(Ax,Ay,Bx,By, matrix.Color333(0,0,0));
  matrix.drawLine(Bx,By,Cx,Cy, matrix.Color333(0,0,0));
  matrix.drawLine(Cx,Cy,Dx,Dy, matrix.Color333(0,0,0));
  matrix.drawLine(Dx,Dy,Ex,Ey, matrix.Color333(0,0,0));
  matrix.drawLine(Ex,Ey,Ax,Ay, matrix.Color333(0,0,0));
  //fill in
  matrix.drawLine(Bx,By,Bx,By+10, matrix.Color333(0,0,0));
  matrix.drawLine(Bx-1,By+2,Bx-1,By+10, matrix.Color333(0,0,0));
  matrix.drawLine(Bx+1,By+2,Bx+1,By+10, matrix.Color333(0,0,0));
  matrix.drawLine(Bx-2,By+9,Bx-2,By+10, matrix.Color333(0,0,0));
  matrix.drawLine(Bx+2,By+9,Bx+2,By+10, matrix.Color333(0,0,0));
  matrix.drawPixel(Bx-3,By+7, matrix.Color333(0,0,0));
  matrix.drawPixel(Bx+3,By+7, matrix.Color333(0,0,0));
}

void setup() {
  pinMode(BOARD_LED_PIN, OUTPUT);
  digitalWrite(BOARD_LED_PIN, LOW);
  pinMode(MAIN_WHEEL, INPUT);

  attachInterrupt(digitalPinToInterrupt(FAST_LOOK_LEFT_BUTTON), fllCallback, RISING);
  attachInterrupt(digitalPinToInterrupt(SLOW_LOOK_LEFT_BUTTON), sllCallback, RISING);
  attachInterrupt(digitalPinToInterrupt(FAST_LOOK_RIGHT_BUTTON), flrCallback, RISING);
  attachInterrupt(digitalPinToInterrupt(SLOW_LOOK_RIGHT_BUTTON), slrCallback, RISING);
  attachInterrupt(digitalPinToInterrupt(FAST_ROLL_EYES_BUTTON), freCallback, RISING);
  attachInterrupt(digitalPinToInterrupt(SLOW_ROLL_EYES_BUTTON), sreCallback, RISING);
  
  for (int i=0; i<window; i++) {
    mainWheelReadings[i] = 0;
    blinkSlideReadings[i] = 0;
  }

  ptr = matrix.backBuffer(); // Get address of matrix data

  // write neutral eye from images header
  memcpy_P(ptr, circle, sizeof(circle));
//  drawStar(16,12);

  Serial.begin(9600);
  matrix.begin();

  

/* asterisks */
//  matrix.setTextColor(matrix.Color333(0,0,0));
//  matrix.setTextSize(2);
  
  for(int i=11; i<21; i++) {
    memcpy_P(ptr, circle, sizeof(circle));
    drawStar(i,12);
    Serial.print("\n----");
    Serial.print(i);
    Serial.print("----");
    matrix.dumpMatrix();
  }
/* end asterisks */

//  matrix.fillCircle(16, 16, 14, matrix.Color333(0, 0, 1));
//  matrix.fillCircle(19, 18, 6, matrix.Color333(0, 0, 0));
//  Serial.println("circle:");
//  matrix.dumpMatrix();
}

void loop() {
//  matrix.fillCircle(16,16,14, matrix.Color333(0,0,1));
//  memcpy_P(ptr, circle, sizeof(circle));
//  x_position = ((mainWheelAvg * 10) / mainWheelMax) + 11;
//  drawStar(x_position,12);
////  Serial.print("X: ");
////  Serial.println(x_position);
////  matrix.setCursor(x_position,12);
////  matrix.print('*');
//  delay(10);
  
//  blinkSlide = analogRead(BLINK_SLIDER);
//  blinkSlideTotal = blinkSlideTotal - blinkSlideReadings[readIdx];
//  blinkSlideTotal = blinkSlideTotal + blinkSlide;
//  blinkSlideReadings[readIdx] = blinkSlide;
//  blinkSlideAvg = blinkSlideTotal / window;
//  Serial.println(blinkSlideAvg);
  
  mainWheel = analogRead(MAIN_WHEEL);
  mainWheelTotal = mainWheelTotal - mainWheelReadings[readIdx];
  mainWheelTotal = mainWheelTotal + mainWheel;
  mainWheelReadings[readIdx] = mainWheel;
  mainWheelAvg = mainWheelTotal / window;
//  Serial.println(mainWheelAvg);
//  delay(delayTime);

  readIdx = (readIdx + 1) % window;
}
