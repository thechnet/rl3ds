/*
Dependencies.
*/

#ifndef ARDUINO
#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#endif

/*
Globals.
*/

volatile enum state { IDLE, SCANNING, RESETTING, FAIL_SD, FAIL_SENSOR } state;

/*
Functions.
*/

#include "interface.h"

#ifndef ARDUINO
void delay(int ms)
{
#ifdef WIN32
  Sleep(ms);
#else
  usleep(ms * 1000 /* microseconds */ );
#endif
}
#endif

void fail(enum state s)
{
  state = s;
  showState();
  while (true);
}

#include "storage.h"

#include "scan.h"

#include "sensor.h"

#include "motors.h"

void setup()
{
  setupInterface();
  setupStorage();
  setupSensor();
  setupMotors();

// #define CALIBRATION_MODE
#ifdef CALIBRATION_MODE
  /* Lower sensor. */ digitalWrite(PIN_DIR_TOWER, HIGH);
  // /* Raise sensor. */ digitalWrite(PIN_DIR_TOWER, LOW);
  for (int s = 0; s < 100 /* Number of steps to move per reset. */; ++s) {
    digitalWrite(PIN_STEP_TOWER, HIGH);
    delayMicroseconds(TOWER_STEP_HALF_DELAY_uS);
    digitalWrite(PIN_STEP_TOWER, LOW);
    delayMicroseconds(TOWER_STEP_HALF_DELAY_uS);
  }
#endif
}

void loop()
{
#ifndef CALIBRATION_MODE
  if (state == SCANNING) {
    showState();
    scan();
  }

  state = IDLE;
  showState();
#endif
}

#ifndef ARDUINO
int main()
{
  setup();
  toggleState();
  loop();
}
#endif
