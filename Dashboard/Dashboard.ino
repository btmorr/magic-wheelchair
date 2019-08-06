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

void ledOn() {
  digitalWrite(BOARD_LED_PIN, HIGH);  
}

void ledOff() {
  digitalWrite(BOARD_LED_PIN, LOW);  
}

void setup() {
  pinMode(BOARD_LED_PIN, OUTPUT);
  digitalWrite(BOARD_LED_PIN, LOW);
//  pinMode(FAST_LOOK_LEFT_BUTTON, INPUT);
  attachInterrupt(digitalPinToInterrupt(FAST_LOOK_LEFT_BUTTON), ledOn, RISING);
//  attachInterrupt(digitalPinToInterrupt(FAST_LOOK_LEFT_BUTTON), ledOff, FALLING); 
}

void loop() {
  delay(1);
}
