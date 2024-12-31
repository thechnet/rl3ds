/*
Constants.
*/

#define PIN_DIR_TABLE 7
#define PIN_STEP_TABLE 6
#define TABLE_STEP_HALF_DELAY_uS 2000 /* 500 is the minimum! */
#define TABLE_STEPS_PER_ROTATION 200
static_assert(TABLE_STEPS_PER_ROTATION % TABLE_STOPS_PER_ROTATION == 0);
#define TABLE_STEPS_PER_STOP (TABLE_STEPS_PER_ROTATION / TABLE_STOPS_PER_ROTATION)

#define PIN_DIR_TOWER 4
#define PIN_STEP_TOWER 5
#define TOWER_STEP_HALF_DELAY_uS 2000
#define TOWER_HEIGHT_IN_STEPS 600
static_assert(TOWER_HEIGHT_IN_STEPS % TOWER_STOPS == 0);
#define TOWER_STEPS_PER_STOP (TOWER_HEIGHT_IN_STEPS / TOWER_STOPS)

/*
Functions.
*/

void setupMotors()
{
#ifdef ARDUINO
  pinMode(PIN_STEP_TABLE, OUTPUT); 
  pinMode(PIN_DIR_TABLE, OUTPUT);
  pinMode(PIN_STEP_TOWER, OUTPUT);
  pinMode(PIN_DIR_TOWER, OUTPUT);

  digitalWrite(PIN_DIR_TABLE, HIGH);
#endif
}

void tableAdvanceToNextStop()
{
#ifdef ARDUINO
  for (int i = 0; i < TABLE_STEPS_PER_STOP; ++i) {
    digitalWrite(PIN_STEP_TABLE, HIGH);
    delayMicroseconds(TABLE_STEP_HALF_DELAY_uS);
    digitalWrite(PIN_STEP_TABLE, LOW); 
    delayMicroseconds(TABLE_STEP_HALF_DELAY_uS);
  }
#else
  delay(2 * TABLE_STEP_HALF_DELAY_uS / 1000);
#endif
}

void towerMotorSetRotationDirection(bool high)
{
#ifdef ARDUINO
  digitalWrite(PIN_DIR_TOWER, high ? HIGH : LOW);
#endif
}

void towerMotorAdvanceToNextStop()
{
#ifdef ARDUINO
  for (int s = 0; s < TOWER_STEPS_PER_STOP; ++s) {
    digitalWrite(PIN_STEP_TOWER, HIGH);
    delayMicroseconds(TOWER_STEP_HALF_DELAY_uS);
    digitalWrite(PIN_STEP_TOWER, LOW);
    delayMicroseconds(TOWER_STEP_HALF_DELAY_uS);
  }
#else
  delay(2 * TOWER_STEP_HALF_DELAY_uS / 1000);
#endif
}
