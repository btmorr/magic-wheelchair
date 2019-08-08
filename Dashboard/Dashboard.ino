/*
  - LED attached from pin 13 to ground (on board)
  - button attached to pin 2 from +5V
  - 10K resistor attached to pin 2 from ground
*/

// button pin map (interrupt pins)
#define FAST_LOOK_LEFT_BUTTON  2
#define SLOW_LOOK_LEFT_BUTTON  3
#define FAST_LOOK_RIGHT_BUTTON 18
#define SLOW_LOOK_RIGHT_BUTTON 19
#define FAST_ROLL_EYES_BUTTON  20
#define SLOW_ROLL_EYES_BUTTON  21

// varistor pin map
/*
 * Wheel behavior:
 * 
 * The main wheel should ___
 * The secondary wheel should ____
 */
#define MAIN_WHEEL   A0
#define SEC_WHEEL    A1
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

bool fllButtonPressed = false;
int fllButtonTime = 0;
int debounceTime = 100;

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
  
  for (int i=0; i<window; i++) {
    mainWheelReadings[i] = 0;
    blinkSlideReadings[i] = 0;
  }

  Serial.begin(9600);
}

void loop() {
  delay(100);
  if (fllPressed())
  {
    // why does the light blink twice? and why does it interfere with
    // reading the potentiometer?
    ledOn();
    delay(1);
    ledOff();
  }
  
  blinkSlide = analogRead(BLINK_SLIDER);
  blinkSlideTotal = blinkSlideTotal - blinkSlideReadings[readIdx];
  blinkSlideTotal = blinkSlideTotal + blinkSlide;
  blinkSlideReadings[readIdx] = blinkSlide;
  blinkSlideAvg = blinkSlideTotal / window;
  
  mainWheel = analogRead(MAIN_WHEEL);
  mainWheelTotal = mainWheelTotal - mainWheelReadings[readIdx];
  mainWheelTotal = mainWheelTotal + mainWheel;
  mainWheelReadings[readIdx] = mainWheel;
  mainWheelAvg = mainWheelTotal / window;

  Serial.println(mainWheelAvg);

  readIdx = (readIdx + 1) % window;
}
