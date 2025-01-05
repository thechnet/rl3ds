/*
Dependencies.
*/

#include <math.h>

/*
Constants.
*/

#define TABLE_STOPS_PER_ROTATION 200
#define TABLE_STOP_DELAY_MS 20 /* Also consider SENSOR_TIMING_BUDGET_MS in sensor.h. */
#define TABLE_CENTER_DISTANCE_FROM_SENSOR_CM ((double)14)
#define MIN_RADIUS_CM ((double)0.01)

#define TOWER_HEIGHT_CM ((double)15) /* The full height of the tower. (!) This is only used to calculate vertex coordinates. The physical size of the tower is given as TOWER_HEIGHT_IN_STEPS in motors.h. */
#define TOWER_LIMIT_CM ((double)5) /* The (actual!) height limit of the scan. */
static_assert(TOWER_LIMIT_CM <= TOWER_HEIGHT_CM);
#define TOWER_HEIGHT_STOPS 25 /* How often the sensor moves up during a scan up to TOWER_HEIGHT_CM. (!) The number of scanned disks is one larger. */
#define TOWER_LIMIT_STOPS ((int)(TOWER_HEIGHT_STOPS * TOWER_LIMIT_CM / TOWER_HEIGHT_CM))
#define TOWER_LOWER_DELAY_PER_STOP_MS 0

#undef M_PI
#define M_PI ((double)3.14159265358979323846)
#define VERTEX_COMPONENT_FORMAT "%.2f"

/*
Globals.
*/

int numberOfVisitedTableStopsInThisRotation;
int numberOfCompletedTowerStops;

/*
Forward-declare some motor functions.
*/

double readDistance_mm();
void tableAdvanceToNextStop();
void towerMotorSetRotationDirection(bool value);
void towerMotorAdvanceToNextStop();

/*
Functions.
*/

void advanceTableThenEmitVertex()
{
  /* Advance table. */

  tableAdvanceToNextStop();
  ++numberOfVisitedTableStopsInThisRotation;

  delay(TABLE_STOP_DELAY_MS); /* Give the distance sensor some time. */

  /* Emit vertex. */

  double theta = M_PI * 2 * numberOfVisitedTableStopsInThisRotation / TABLE_STOPS_PER_ROTATION;
  double z = TOWER_LIMIT_CM * numberOfCompletedTowerStops / TOWER_LIMIT_STOPS;

  double distance_cm = readDistance_mm() / 10;
  double radius_cm = TABLE_CENTER_DISTANCE_FROM_SENSOR_CM - distance_cm;
  double normalizedRadius_cm = radius_cm < MIN_RADIUS_CM ? 0 : radius_cm;
  double x = normalizedRadius_cm * cos(theta);
  double y = normalizedRadius_cm * sin(theta);

  emit("v " VERTEX_COMPONENT_FORMAT " " VERTEX_COMPONENT_FORMAT " " VERTEX_COMPONENT_FORMAT "\n", x, z, y);
}

void rotateThenEmitFaces()
{
  /* Rotate. */

  numberOfVisitedTableStopsInThisRotation = 0;
  for (int s = 0; s < TABLE_STOPS_PER_ROTATION; ++s) {
    if (state != SCANNING)
      return;
    advanceTableThenEmitVertex();
  }

  if (numberOfCompletedTowerStops < 1) /* If this is the first disk we can't form any faces yet. */
    return;

  /* Emit faces. */

  int firstUpperVertex = 1 + TABLE_STOPS_PER_ROTATION * numberOfCompletedTowerStops;
  int firstLowerVertex = firstUpperVertex - TABLE_STOPS_PER_ROTATION;

  for (int i = 0; i < TABLE_STOPS_PER_ROTATION; i++) {
    int iPlus1 = (i + 1) % TABLE_STOPS_PER_ROTATION;
    emit("f %d %d %d\n", firstLowerVertex + i, firstLowerVertex + iPlus1, firstUpperVertex + i);
    emit("f %d %d %d\n", firstUpperVertex + i, firstLowerVertex + iPlus1, firstUpperVertex + iPlus1);
  }
}

void scan()
{
  openResultFile();

  towerMotorSetRotationDirection(false);
  numberOfCompletedTowerStops = 0;
  while (true) {
    rotateThenEmitFaces();
    if (numberOfCompletedTowerStops++ >= TOWER_LIMIT_STOPS || state != SCANNING)
      break;
    towerMotorAdvanceToNextStop();
  }

  state = RESETTING;
  showState();

  towerMotorSetRotationDirection(true);
  while (--numberOfCompletedTowerStops > 0) {
    towerMotorAdvanceToNextStop();
    delay(TOWER_LOWER_DELAY_PER_STOP_MS);
  }

  closeResultFile();
}
