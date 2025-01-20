#undef USE_VL53L1X
#ifdef ARDUINO
#define USE_VL53L1X
#endif

/*
While using this, change the following values to match the configuration used to collect the distances:
- set TABLE_STEPS_PER_ROTATION to 224 (to allow the following value)
- set TABLE_STOPS_PER_ROTATION to 32
- set TABLE_CENTER_DISTANCE_FROM_SENSOR_CM to 7.2
*/
// #define USE_HARDCODED_DISTANCES

/*
Constants.
*/

#define SENSOR_TIMING_BUDGET_MS 300 /* How long the sensor is allowed to take for a measurement. Minimum in short mode is 20 ms. */
#define SENSOR_PERIOD_MS SENSOR_TIMING_BUDGET_MS /* The inter-measurement period. Minimum is the timing budget. */

/*
Dependencies.
*/

#ifdef USE_VL53L1X
#include <Wire.h>
#include "VL53L1X/VL53L1X.h" /* http://www.cqrobot.wiki/index.php/VL53L1X_Distance_Ranging_Sensor_SKU:_CQRWX00744US */
#include "VL53L1X/VL53L1X.cpp"
#else
#include <stdlib.h>
#endif

/*
Globals.
*/

#ifdef USE_VL53L1X
VL53L1X distanceSensor;
#endif

/*
Functions.
*/

void setupSensor()
{
#ifdef USE_VL53L1X
  Wire.begin();
  Wire.setClock(400000);
  distanceSensor.setTimeout(500); /* The maximum number of milliseconds to stall in case of a sensor failure. */
  if (!distanceSensor.init()) {
    log("Cannot initialize distance sensor!\n");
    fail(FAIL_SENSOR);
  }
  distanceSensor.setDistanceMode(VL53L1X::Short); /* Short mode tests up to 1.3 m. */
  distanceSensor.setMeasurementTimingBudget(SENSOR_TIMING_BUDGET_MS * 1000 /* microseconds */);
  distanceSensor.startContinuous(SENSOR_PERIOD_MS); /* The sensor updates continously to lower latency when fetching using .read(). */
#else
  srand(1);
#endif
}

double readDistance_mm()
{
#ifdef USE_VL53L1X
  distanceSensor.read(); /* Fetch the latest measurement. */
  delay(SENSOR_TIMING_BUDGET_MS);
  double distance_mm = distanceSensor.ranging_data.range_status == VL53L1_RANGESTATUS_RANGE_VALID ? distanceSensor.ranging_data.range_mm : INFINITY;
  // log("%.2f (%s)\n", distance_mm, VL53L1X::rangeStatusToString(distanceSensor.ranging_data.range_status));
#elif defined(USE_HARDCODED_DISTANCES)
  static double distances[] = { 4.7, 4.65, 4.5, 4.25, 3.65, 4.25, 4.5, 4.65, 4.7, 4.65, 4.5, 4.25, 3.65, 4.15, 8.9, 8.75, 8.8, 8.75, 8.9, 4.15, 3.65, 4.25, 4.5, 4.65, 4.7, 4.65, 4.5, 4.25, 3.65, 4.25, 4.5, 4.65 };
  static int distanceIndex = 0;
  double distance_mm = distanceIndex < sizeof(distances) / sizeof(*distances) ? distances[distanceIndex++] * 10 : INFINITY;
#else
  double distance_mm = TABLE_CENTER_DISTANCE_FROM_SENSOR_CM * 10 / 2 + (-5 + ((double)rand() / RAND_MAX) * TABLE_CENTER_DISTANCE_FROM_SENSOR_CM * 10 / 20);
#endif
  return distance_mm;
}
