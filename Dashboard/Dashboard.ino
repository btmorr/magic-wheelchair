#include <RGBmatrixPanel.h>
#include <gamma.h>

#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>

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
//#define MAIN_WHEEL   A0
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
#define BLINK_SLIDER A2

// other pins
#define BOARD_LED_PIN 13

const int window = 4;
int readIdx = 0;

int mainWheel = 0;
int mainWheelReadings[window];
int mainWheelTotal = 0;
int mainWheelAvg = 0;

int blinkSlide = 0;
int blinkSlideReadings[window];
int blinkSlideTotal = 0;
int blinkSlideAvg = 0;
int blinkSlideMax = 14000;
int debounceTime = 100;
int delayTime = 0;

#define FAST_LOOK_LEFT_BUTTON  2   // fll
bool fllButtonPressed = false;
unsigned long fllButtonTime = 0;

void fllCallback()
{
  if(digitalRead(FAST_LOOK_LEFT_BUTTON) == LOW)
  {
    fllButtonPressed = true;
    fllButtonTime = millis();
  }
}

bool fllPressed()
{
  if(fllButtonPressed && millis() - fllButtonTime >= debounceTime)
  {
    fllButtonPressed = false;
    return true;
  }
  return false;
}

#define SLOW_LOOK_LEFT_BUTTON  3   // sll
bool sllButtonPressed = false;
unsigned long sllButtonTime = 0;

void sllCallback()
{
  if(digitalRead(SLOW_LOOK_LEFT_BUTTON) == LOW)
  {
    sllButtonPressed = true;
    sllButtonTime = millis();
  }
}

bool sllPressed()
{
  if(sllButtonPressed && millis() - sllButtonTime >= debounceTime)
  {
    sllButtonPressed = false;
    return true;
  }
  return false;
}

#define FAST_LOOK_RIGHT_BUTTON 18  // flr
bool flrButtonPressed = false;
unsigned long flrButtonTime = 0;

void flrCallback()
{
  if(digitalRead(FAST_LOOK_RIGHT_BUTTON) == LOW)
  {
    flrButtonPressed = true;
    flrButtonTime = millis();
  }
}

bool flrPressed()
{
  if(flrButtonPressed && millis() - flrButtonTime >= debounceTime)
  {
    flrButtonPressed = false;
    return true;
  }
  return false;
}

#define SLOW_LOOK_RIGHT_BUTTON 19  // slr
bool slrButtonPressed = false;
unsigned long slrButtonTime = 0;

void slrCallback()
{
  if(digitalRead(SLOW_LOOK_RIGHT_BUTTON) == LOW)
  {
    slrButtonPressed = true;
    slrButtonTime = millis();
  }
}

bool slrPressed()
{
  if(slrButtonPressed && millis() - slrButtonTime >= debounceTime)
  {
    slrButtonPressed = false;
    return true;
  }
  return false;
}

#define FAST_ROLL_EYES_BUTTON  20  // fre
bool freButtonPressed = false;
unsigned long freButtonTime = 0;

void freCallback()
{
  if(digitalRead(FAST_ROLL_EYES_BUTTON) == LOW)
  {
    freButtonPressed = true;
    freButtonTime = millis();
  }
}

bool frePressed()
{
  if(freButtonPressed && millis() - freButtonTime >= debounceTime)
  {
    freButtonPressed = false;
    return true;
  }
  return false;
}

#define SLOW_ROLL_EYES_BUTTON  21  // sre
bool sreButtonPressed = false;
unsigned long sreButtonTime = 0;

void sreCallback()
{
  if(digitalRead(SLOW_ROLL_EYES_BUTTON) == LOW)
  {
    sreButtonPressed = true;
    sreButtonTime = millis();
  }
}

bool srePressed()
{
  if(sreButtonPressed && millis() - sreButtonTime >= debounceTime)
  {
    sreButtonPressed = false;
    return true;
  }
  return false;
}

void ledOn()
{
  digitalWrite(BOARD_LED_PIN, HIGH);
}

void ledOff()
{
  digitalWrite(BOARD_LED_PIN, LOW);
}

void setup() {
  pinMode(BOARD_LED_PIN, OUTPUT);
  digitalWrite(BOARD_LED_PIN, LOW);

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

  Serial.begin(9600);
  matrix.begin();
  
  matrix.fillCircle(16, 16, 14, matrix.Color333(0, 0, 1));
  matrix.fillCircle(19, 18, 6, matrix.Color333(0, 0, 0));
}

void loop() {
  matrix.fillCircle(16, 16, 14, matrix.Color333(0, 0, 1));
  matrix.fillCircle(19, 18, 6, matrix.Color333(0, 0, 0));

  if (fllPressed())
  {
    // fast look left
    matrix.fillCircle(16, 16, 15, matrix.Color333(0, 0, 1));
    matrix.fillCircle(8, 16, 6, matrix.Color333(0, 0, 0));
    delayTime = 100;
  } else if (sllPressed()) {
    // slow look left
    matrix.fillCircle(16, 16, 15, matrix.Color333(0, 0, 1));
    matrix.fillCircle(8, 16, 6, matrix.Color333(0, 0, 0));
    delayTime = 1000;
  } else if (flrPressed()) {
    // fast look right
    matrix.fillCircle(16, 16, 15, matrix.Color333(0, 0, 1));
    matrix.fillCircle(8, 16, 6, matrix.Color333(0, 0, 0));
    delayTime = 100;
  } else if (slrPressed()) {
    // slow look right
    matrix.fillCircle(16, 16, 15, matrix.Color333(0, 0, 1));
    matrix.fillCircle(8, 16, 6, matrix.Color333(0, 0, 0));
    delayTime = 1000;
  } else if (frePressed()) {
    // fast roll eyes
    matrix.fillCircle(16, 16, 15, matrix.Color333(0, 0, 1));
    matrix.fillCircle(8, 16, 6, matrix.Color333(0, 0, 0));
    delayTime = 100;
  } else if (srePressed()) {
    // slow roll eyes
    matrix.fillCircle(16, 16, 15, matrix.Color333(0, 0, 1));
    matrix.fillCircle(8, 16, 6, matrix.Color333(0, 0, 0));
    delayTime = 1000;
  }
  delay(delayTime);
  
  blinkSlide = analogRead(BLINK_SLIDER);
  blinkSlideTotal = blinkSlideTotal - blinkSlideReadings[readIdx];
  blinkSlideTotal = blinkSlideTotal + blinkSlide;
  blinkSlideReadings[readIdx] = blinkSlide;
  blinkSlideAvg = blinkSlideTotal / window;
  Serial.println(blinkSlideAvg);
  
//  mainWheel = analogRead(MAIN_WHEEL);
//  mainWheelTotal = mainWheelTotal - mainWheelReadings[readIdx];
//  mainWheelTotal = mainWheelTotal + mainWheel;
//  mainWheelReadings[readIdx] = mainWheel;
//  mainWheelAvg = mainWheelTotal / window;

  readIdx = (readIdx + 1) % window;
}
