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
}

void loop()
{
  if (state == SCANNING) {
    showState();
    scan();
  }

  state = IDLE;
  showState();
}

#ifndef ARDUINO
int main()
{
  setup();
  toggleState();
  loop();
}
#endif
