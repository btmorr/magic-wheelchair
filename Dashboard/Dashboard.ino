/******* DFPlayer mini *******/
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

/**** RGB Matrix LED Panel ****/
#include <RGBmatrixPanel.h>
#include <gamma.h>
#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>

/**** Dashboard hearder files ****/
#include "images.h"
#include "buttons.h"
#include "hearteyes.h"
#include "roundeyes.h"
#include "stareyes.h"
#include "blinkeyes.h"

/* Attach a 9DOF sensor in the steering wheel to get wheel's anglular position */
#ifdef USE_9DOF_SENSOR
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_L3GD20_U.h>
#include <Adafruit_9DOF.h>

/* Assign a unique ID to the sensors */
Adafruit_9DOF                dof   = Adafruit_9DOF();
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(30301);
Adafruit_LSM303_Mag_Unified   mag   = Adafruit_LSM303_Mag_Unified(30302);

/* Update this with the correct SLP for accurate altitude measurements */
float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;
float wheel_angle = 0;

void initSensors()
{
  if(!accel.begin())
  {
    /* There was a problem detecting the LSM303 ... check your connections */
    Serial.println(F("Ooops, no LSM303 detected ... Check your wiring!"));
    while(1);
  }
  if(!mag.begin())
  {
    /* There was a problem detecting the LSM303 ... check your connections */
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while(1);
  }
}
#endif
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
#define IMG_SIZE   1536

#define MAIN_WHEEL    A8

#define CIRCLE  0
#define STAR    1
#define HEART   2

#define SW_SERIAL_RX  A11
#define SW_SERIAL_TX  A12
#define VOLUME_CNTRL  A5
#define VOLUME_UPDATE_INTERVAL 500
#define MEOW_INTERVAL 60000

#define CIRCLE_EYE_TRACK    2
#define STAR_EYE_TRACK      3
#define HEART_EYE_TRACK     4
#define BLINK_EYE_TRACK     5
#define RANDOM_MEOWS_TRACK  1

SoftwareSerial mySoftwareSerial(SW_SERIAL_RX, SW_SERIAL_TX); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

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
  mySoftwareSerial.begin(9600);
  Serial.begin(115200);

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

#ifdef USE_9DOF_SENSOR
  /* Initialise the sensors */
  initSensors();
#endif

  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online."));

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

  //  Serial terminal--uncomment to add and read serial debug messages
  //  Serial.begin(9600);
  //  Serial.println("start");
  matrix.begin();
  myDFPlayer.volume(get_volume());  //Set volume value. From 0 to 30
}

void loop() {

  update_volume();  // Every VOLUME_UPDATE_INTERVAL msec from on-board potentiometer

  if (circlePressed()) {
    Serial.println("circle");
    myDFPlayer.playFolder(CIRCLE_EYE_TRACK, CIRCLE_EYE_TRACK);
    eyeShape = CIRCLE;
 }
 else if (starPressed()) {
   Serial.println("star");
   myDFPlayer.playFolder(STAR_EYE_TRACK, STAR_EYE_TRACK);
   eyeShape = STAR;
  }
  else if (heartPressed()) {
    Serial.println("heart");
    myDFPlayer.playFolder(HEART_EYE_TRACK, HEART_EYE_TRACK);
    eyeShape = HEART;
  }
  else if (blinkPressed()) {
    Serial.println("blink");
    myDFPlayer.playFolder(BLINK_EYE_TRACK, BLINK_EYE_TRACK);
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

  meow_occasionally();

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

#ifdef USE_9DOF_SENSOR
  sensors_event_t accel_event;
  sensors_event_t mag_event;
  sensors_vec_t   orientation;

  /* Calculate pitch and roll from the raw accelerometer data */
  accel.getEvent(&accel_event);
  dof.accelGetOrientation(&accel_event, &orientation) // orientation.pitch, orientation.roll
  /* Calculate the heading using the magnetometer */
  mag.getEvent(&mag_event);
  if (dof.magGetOrientation(SENSOR_AXIS_Z, &mag_event, &orientation))
  {
    // returns value between 0 - 360
    wheel_angle = orientation.heading;
  }
  Serial.println();
  /**** TODO: low pass filter the above value & convert to eye movement ****/

#else
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
#endif

  memcpy_P(ptr, animations[eyeShape][x_position], IMG_SIZE);
  delay(delayTime);

  // Update the ring buffer index variable
  readIdx = (readIdx + 1) % window;

  if (myDFPlayer.available()) {
    // Print the detail message from DFPlayer to handle different errors and states.
    printDetail(myDFPlayer.readType(), myDFPlayer.read());
  }
}


uint8_t get_volume()
{
  return map(analogRead(VOLUME_CNTRL), 0, 1024, 0, 30);
}

void update_volume()
{
  static unsigned long last_vol_update = 0;
  if (millis() - last_vol_update >= VOLUME_UPDATE_INTERVAL)
  {
    myDFPlayer.volume(get_volume());  //Set volume value. From 0 to 30
    last_vol_update = millis();
  }
}

void meow_occasionally()
{
  static unsigned long last_meow = 0;
  if (millis() - last_meow >= MEOW_INTERVAL)
  {
    myDFPlayer.playFolder(RANDOM_MEOWS_TRACK, RANDOM_MEOWS_TRACK);
    last_meow = millis();
  }
}

void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }

}
