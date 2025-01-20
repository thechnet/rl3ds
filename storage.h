/*
Dependencies.
*/

#ifdef ARDUINO
#include <SPI.h>
#include <SD.h>
#include "pins_arduino.h" /* SS_PIN. */
#else
#include <stdio.h>
#endif

// #define NO_SD

/*
Constants.
*/

#define PIN_SD 8
#define RESULT_FILE_NAME "scan.obj" // TODO: Maybe use the date and time here.

/*
Globals.
*/

#ifdef ARDUINO
File resultFile;
#endif

/*
Functions.
*/

void setupStorage()
{
#if defined(ARDUINO) && !defined(NO_SD)
  pinMode(SS_PIN, OUTPUT); /* See SD documentation. */
  // pinMode(PIN_SD, OUTPUT); // FIXME: Is this required?

  if (!SD.begin(PIN_SD)) {
    log("Cannot initialize SD card!\n");
    fail(FAIL_SD);
  }
#endif
}

void openResultFile()
{
#if defined(ARDUINO) && !defined(NO_SD)
  if (SD.exists(RESULT_FILE_NAME) && !SD.remove(RESULT_FILE_NAME)) {
    log("Cannot delete %s!\n", RESULT_FILE_NAME);
    fail(FAIL_SD);
  }
  resultFile = SD.open(RESULT_FILE_NAME, FILE_WRITE);
  if (!resultFile) {
    log("Cannot open %s!\n", RESULT_FILE_NAME);
    fail(FAIL_SD);
  }
#endif
}

void closeResultFile()
{
#if defined(ARDUINO) && !defined(NO_SD)
  resultFile.close();
#endif
}

void emit(const char *format, ...)
{
  char text[256];
  va_list args;
  va_start(args, format);
  vsnprintf(text, sizeof(text), format, args);
  va_end(args);
#ifdef ARDUINO
#ifndef NO_SD
  resultFile.print(text);
#endif
#else
  printf("%s", text);
#endif
  // log(text);
}
