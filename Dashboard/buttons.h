// TODO: rename constants and functions to reflect new button behaviors

int debounceTime = 100;

#define FAST_LOOK_LEFT_BUTTON  2   // circle eyes
volatile bool fllButtonPressed = false;
volatile unsigned long fllButtonTime = 0;

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

#define SLOW_LOOK_LEFT_BUTTON  3   // star eyes
volatile bool sllButtonPressed = false;
volatile unsigned long sllButtonTime = 0;

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

#define FAST_LOOK_RIGHT_BUTTON 18  // heart eyes
volatile bool flrButtonPressed = false;
volatile unsigned long flrButtonTime = 0;

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

#define SLOW_LOOK_RIGHT_BUTTON 19  // blink
volatile bool slrButtonPressed = false;
volatile unsigned long slrButtonTime = 0;

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

#define FAST_ROLL_EYES_BUTTON  21  // roll eyes
volatile bool freButtonPressed = false;
volatile unsigned long freButtonTime = 0;

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

#define SLOW_ROLL_EYES_BUTTON  20  // sre
volatile bool sreButtonPressed = false;
volatile unsigned long sreButtonTime = 0;

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
