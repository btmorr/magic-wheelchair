#include <RGBmatrixPanel.h>
#include <gamma.h>

#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>

#include "images.h"
#include "buttons.h"
#include "hearteyes.h"
#include "roundeyes.h"
#include "stareyes.h"
#include "blinkeyes.h"

/*
 * For each button:
 * - button attached to Arduino pin with internal pull-up resistor
 * - other end of button connected to ground
 *    +5V
 *    |
 *    |
 *    X 10k   <- Internal Pull-up resistor
 *    X
 *    |________    <- Arduino Pin
 *             |
 *              /  <- Button Switch
 *             |
 *            _|_  <- Gnd
 *             ¯
 */
/* pin map

[EYES] OE (output enable)         9
[EYES] LAT (latch)               10
[EYES] CLK (clock for Mega)      11
[EYES] blue1 (upper) output      26
[EYES] blue2 (bottom) output     29

[EYES] row select A              A0
[EYES] row select B              A1
[EYES] row select C              A2
[EYES] row select D              A3

All eye pins not listed above are grounded

round eyes button                 2
star eyes button                  3
heart eyes button                18
blink button                     19
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

#define IMG_SIZE   1536

#define CIRCLE  0
#define STAR    1
#define HEART   2
int eyeShape = CIRCLE;

// Regular eye movement animation cells for round, star, and heart pupils
const uint8_t* animations[3][10] = {
  { roundEye0, roundEye1, roundEye2, roundEye3, roundEye4, roundEye5, roundEye6, roundEye7, roundEye8, roundEye9 },
  { star0, star1, star2, star3, star4, star5, star6, star7, star8, star9 },
  { heartEye0, heartEye1, heartEye2, heartEye3, heartEye4, heartEye5, heartEye6, heartEye7, heartEye8, heartEye9 }
};

// Blink animation cells. Not all are used--some seem to cause an error (I'm looking at you, blink5)
const uint8_t* blinkCells[12] = {
  blink0, blink1, blink2, blink3, blink4, blink5,
  blink6, blink7, blink8, blink9, blink10, blink11
};

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
  // Set button and wheel pins as inputs
  pinMode(MAIN_WHEEL, INPUT);
  pinMode(CIRCLE_EYE_BUTTON, INPUT_PULLUP);
  pinMode(STAR_EYE_BUTTON, INPUT_PULLUP);
  pinMode(HEART_EYE_BUTTON, INPUT_PULLUP);
  pinMode(BLINK_BUTTON, INPUT_PULLUP);

  // Hook up the interrupt function for each button
  attachInterrupt(digitalPinToInterrupt(CIRCLE_EYE_BUTTON), circleCallback, FALLING);
  attachInterrupt(digitalPinToInterrupt(STAR_EYE_BUTTON), starCallback, FALLING);
  attachInterrupt(digitalPinToInterrupt(HEART_EYE_BUTTON), heartCallback, FALLING);
  attachInterrupt(digitalPinToInterrupt(BLINK_BUTTON), blinkCallback, FALLING);

  // Initialize the ring buffer for the eyes
  for (int i=0; i<window; i++) {
    mainWheelReadings[i] = 0;
  }

  ptr = matrix.backBuffer(); // Get address of matrix data

  // write eye background from images header
  memcpy_P(ptr, circle, IMG_SIZE);

  // Serial terminal--uncomment to add and read serial debug messages
//  Serial.begin(9600);
//  Serial.println("start");
  matrix.begin();
}

void loop() {
  if (circlePressed()) {
    Serial.println("circle");
    eyeShape = CIRCLE;
 } else if (starPressed()) {
   Serial.println("star");
   eyeShape = STAR;
  } else if (heartPressed()) {
    Serial.println("heart");
    eyeShape = HEART;
  } else if (blinkPressed()) {
    Serial.println("blink");
    memcpy_P(ptr, blinkCells[1], IMG_SIZE);
    delay(15);
    memcpy_P(ptr, blinkCells[4], IMG_SIZE);
    delay(15);
    memcpy_P(ptr, blinkCells[8], IMG_SIZE);
    delay(15);
    memcpy_P(ptr, blinkCells[10], IMG_SIZE);
    delay(15);
    memcpy_P(ptr, blinkCells[11], IMG_SIZE);
    delay(15);
    memcpy_P(ptr, blinkCells[10], IMG_SIZE);
    delay(15);
    memcpy_P(ptr, blinkCells[8], IMG_SIZE);
    delay(15);
    memcpy_P(ptr, blinkCells[4], IMG_SIZE);
    delay(15);
    memcpy_P(ptr, blinkCells[1], IMG_SIZE);
    delay(15);
  }

  /* Main wheel is read synchronously on each loop, and then
   * a low-pass filter (average of a ring buffer) is applied
   * to smooth out movement. A larger window results in smoother
   * movement, but slower response. A smaller window is more
   * responsive, but jittery. Note that this is a combination of
   * the window size and also the delay time. Window size must
   * be constant, but delay time can be dynamic, and could be
   * changed at runtime if you wanted to alter the animation to
   * be more jittery (caffeinated?) vs laggy (sleepy?).
   */

  unsigned long timeStamp = millis();
  // Read the value from the analog-to-digital-converter (value
  // will be between 0 and 1023
  mainWheel = analogRead(MAIN_WHEEL);
  // Drop the oldest value in the ring buffer from the total
  mainWheelTotal = mainWheelTotal - mainWheelReadings[readIdx];
  // Add the newly read value to the total
  mainWheelTotal = mainWheelTotal + mainWheel;
  // Overwrite the oldest value in the ring buffer with the new one
  mainWheelReadings[readIdx] = mainWheel;
  // Get the average
  mainWheelAvg = mainWheelTotal / window;

  // for each pupil shape, there are 10 cells for lateral movement,
  // so the full range of wheel input is normalized to [0-9]
  x_position = ((mainWheelAvg * 9) / mainWheelMax);

  memcpy_P(ptr, animations[eyeShape][x_position], IMG_SIZE);
  delay(delayTime);

  // Update the ring buffer index variable
  readIdx = (readIdx + 1) % window;
  timeStamp = millis() - timeStamp;
  Serial.print("Loop time:"); Serial.println(timeStamp);
}
