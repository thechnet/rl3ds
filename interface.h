/*
Dependencies.
*/

#include <stdarg.h>
#ifdef ARDUINO
#include <Arduino_LED_Matrix.h>
#else
#include <stdio.h>
#include <stdint.h>
#endif

/*
Constants.
*/

#define PIN_PUSHBUTTON 2

/*
Types.
*/

#ifndef ARDUINO
typedef uint8_t byte;
#endif

/*
Globals.
*/

#ifdef ARDUINO
ArduinoLEDMatrix ledMatrix;
#endif

#define M 1
#define _ 0
  byte bitmapIdle[8][12] = {
    { _, _, _, _, _, _, _, _, _, _, _, _ },
    { _, M, M, M, _, _, _, _, _, _, _, _ },
    { _, _, M, _, _, _, _, _, _, _, _, _ },
    { _, _, M, _, _, _, _, _, _, _, _, _ },
    { _, _, M, _, _, _, _, _, _, _, _, _ },
    { _, M, M, M, _, _, _, _, _, _, _, _ },
    { _, _, _, _, _, _, _, _, _, _, _, _ },
    { _, _, _, _, _, _, _, _, _, _, _, _ },
  };
  byte bitmapScanning[8][12] = {
    { _, _, _, _, _, _, _, _, _, _, _, _ },
    { _, M, M, M, _, _, _, _, _, _, _, _ },
    { _, M, _, _, _, _, _, _, _, _, _, _ },
    { _, M, M, M, _, _, _, _, _, _, _, _ },
    { _, _, _, M, _, _, _, _, _, _, _, _ },
    { _, M, M, M, _, _, _, _, _, _, _, _ },
    { _, _, _, _, _, _, _, _, _, _, _, _ },
    { _, _, _, _, _, _, _, _, _, _, _, _ },
  };
  byte bitmapResetting[8][12] = {
    { _, _, _, _, _, _, _, _, _, _, _, _ },
    { _, M, M, _, _, _, _, _, _, _, _, _ },
    { _, M, _, M, _, _, _, _, _, _, _, _ },
    { _, M, M, _, _, _, _, _, _, _, _, _ },
    { _, M, _, M, _, _, _, _, _, _, _, _ },
    { _, M, _, M, _, _, _, _, _, _, _, _ },
    { _, _, _, _, _, _, _, _, _, _, _, _ },
    { _, _, _, _, _, _, _, _, _, _, _, _ },
  };
  byte bitmapFailed[8][12] = {
    { _, _, _, _, _, _, _, _, _, _, _, _ },
    { _, M, M, M, _, _, _, _, _, _, _, _ },
    { _, M, _, _, _, _, _, _, _, _, _, _ },
    { _, M, M, M, _, _, _, _, _, _, _, _ },
    { _, M, _, _, _, _, _, _, _, _, _, _ },
    { _, M, _, _, _, _, _, _, _, _, _, _ },
    { _, _, _, _, _, _, _, _, _, _, _, _ },
    { _, _, _, _, _, _, _, _, _, _, _, _ },
  };
#undef M
#undef _

/*
Functions.
*/

void log(const char *format, ...)
{
  char message[256];
  va_list args;
  va_start(args, format);
  vsnprintf(message, sizeof(message) / sizeof(*message), format, args);
  va_end(args);
#ifdef ARDUINO
  Serial.print(message);
#else
  fprintf(stderr, "%s", message);
#endif
}

void showState()
{
  byte (*bitmap)[12];
  switch (state) {
  case IDLE:      bitmap = bitmapIdle;      break;
  case SCANNING:  bitmap = bitmapScanning;  break;
  case RESETTING: bitmap = bitmapResetting; break;
  case FAILED:    bitmap = bitmapFailed;    break;
  }
#ifdef ARDUINO
  ledMatrix.renderBitmap(bitmap, 8, 12);
#else
  log("\n");
  for (int i = 0; i < sizeof(bitmapIdle) / sizeof(*bitmapIdle); ++i) {
    for (int j = 0; j < sizeof(*bitmapIdle) / sizeof(**bitmapIdle); ++j) {
      log("\033[48;5;25m\033[38;5;%dm◢◤\033[0m", bitmap[i][j] ? 210 : 252);
    }
    log("\n");
  }
#endif
}

void toggleState()
{
  switch (state) {
  case IDLE:     state = SCANNING;  break;
  case SCANNING: state = RESETTING; break;
  default: break;
  }
  // TODO: Use a new setState function here that encompasses showState?
}

void setupInterface()
{
#ifdef ARDUINO
  pinMode(PIN_PUSHBUTTON, INPUT);

  attachInterrupt(digitalPinToInterrupt(PIN_PUSHBUTTON), toggleState, FALLING);
  
  Serial.begin(115200);

  ledMatrix.begin();
#endif
}
