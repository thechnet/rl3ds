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

#define BUFFER_CAPACITY 12000
#define PIN_SD 8
#define RESULT_FILE_NAME "scan.obj" // TODO: Maybe use the date and time here.

/*
Globals.
*/

#ifdef ARDUINO
File resultFile;
#endif

char buffer[BUFFER_CAPACITY];
int bufferTail;

/*
Functions.
*/

void flush()
{
#ifdef ARDUINO
#ifndef NO_SD
  resultFile.print(buffer);
#endif
#else
  printf("%s", buffer);
#endif
  // log(buffer);

  bufferTail = 0;
  buffer[bufferTail] = '\0';
}

void emit(const char *format, ...)
{
  char text[256];
  va_list args;
  va_start(args, format);
  int charsWritten = vsnprintf(text, sizeof(text), format, args);
  va_end(args);

  if ((BUFFER_CAPACITY - 1) - bufferTail <= charsWritten)
    flush();
  strcpy(buffer + bufferTail, text);
  bufferTail += charsWritten;
}

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

  flush();
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
