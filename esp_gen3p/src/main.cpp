#include "Arduino.h"
#include "DelayControlValue.h"
DelayControlValue<uint8_t, 8> lampCtrl;

/**
 * Blink
 *
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */

#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

void setup()
{
  // initialize LED digital pin as an output.
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  // turn the LED on (HIGH is the voltage level)
  digitalWrite(LED_BUILTIN, HIGH);

int tAll=600;
int tOn=500;
  // wait for a second
  delay(tAll-tOn);

  // turn the LED off by making the voltage LOW
  digitalWrite(LED_BUILTIN, LOW);

   // wait for a second
  delay(tOn);
}
