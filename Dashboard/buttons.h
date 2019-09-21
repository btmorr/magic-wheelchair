// TODO: rename constants and functions to reflect new button behaviors

int debounceTime = 100;

#define CIRCLE_EYE_BUTTON  2   // circle eyes
volatile bool circleButtonPressed = false;
volatile unsigned long circleButtonTime = 0;

void circleCallback()
{
  if(digitalRead(CIRCLE_EYE_BUTTON) == LOW)
  {
    circleButtonPressed = true;
    circleButtonTime = millis();
  }
}

bool circlePressed()
{
  if(circleButtonPressed && millis() - circleButtonTime >= debounceTime)
  {
    circleButtonPressed = false;
    return true;
  }
  return false;
}

#define STAR_EYE_BUTTON  3   // star eyes
volatile bool starButtonPressed = false;
volatile unsigned long starButtonTime = 0;

void starCallback()
{
  if(digitalRead(STAR_EYE_BUTTON) == LOW)
  {
    starButtonPressed = true;
    starButtonTime = millis();
  }
}

bool starPressed()
{
  if(starButtonPressed && millis() - starButtonTime >= debounceTime)
  {
    starButtonPressed = false;
    return true;
  }
  return false;
}

#define HEART_EYE_BUTTON 18  // heart eyes
volatile bool heartButtonPressed = false;
volatile unsigned long heartButtonTime = 0;

void heartCallback()
{
  if(digitalRead(HEART_EYE_BUTTON) == LOW)
  {
    heartButtonPressed = true;
    heartButtonTime = millis();
  }
}

bool heartPressed()
{
  if(heartButtonPressed && millis() - heartButtonTime >= debounceTime)
  {
    heartButtonPressed = false;
    return true;
  }
  return false;
}

#define BLINK_BUTTON 19  // blink
volatile bool blinkButtonPressed = false;
volatile unsigned long blinkButtonTime = 0;

void blinkCallback()
{
  if(digitalRead(BLINK_BUTTON) == LOW)
  {
    blinkButtonPressed = true;
    blinkButtonTime = millis();
  }
}

bool blinkPressed()
{
  if(blinkButtonPressed && millis() - blinkButtonTime >= debounceTime)
  {
    blinkButtonPressed = false;
    return true;
  }
  return false;
}
