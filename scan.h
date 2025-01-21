/*
Dependencies.
*/

#include <math.h>

/*
Constants.
*/

#define MODEL_POINT_CLOUD
#define POINT_CLOUD_POINT_SIZE_CM ((double)0.1)

#define TABLE_STOPS_PER_ROTATION 50
#define TABLE_CENTER_DISTANCE_FROM_SENSOR_CM ((double)7.5)

#define TOWER_HEIGHT_CM ((double)11.5) /* The full height of the tower. (!) This is only used to calculate vertex coordinates. The physical size of the tower is given as TOWER_HEIGHT_IN_STEPS in motors.h. */
#define TOWER_LIMIT_CM ((double)11.5) /* The (actual!) height limit of the scan. */
static_assert(TOWER_LIMIT_CM <= TOWER_HEIGHT_CM);
#define TOWER_HEIGHT_STOPS 30 /* How often the sensor moves up during a scan up to TOWER_HEIGHT_CM. (!) The number of scanned disks is one larger. */
#define TOWER_LIMIT_STOPS ((int)((double)TOWER_HEIGHT_STOPS * TOWER_LIMIT_CM / TOWER_HEIGHT_CM))
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

  /* Emit vertex. */

  double theta = M_PI * 2 * numberOfVisitedTableStopsInThisRotation / TABLE_STOPS_PER_ROTATION;
  double y = TOWER_LIMIT_CM * numberOfCompletedTowerStops / TOWER_LIMIT_STOPS;

  double distance_cm = readDistance_mm() / 10;
  double radius_cm = TABLE_CENTER_DISTANCE_FROM_SENSOR_CM - distance_cm;

  /* We need a better solution for this. */
  if (isinf(distance_cm)) {
    radius_cm = 0;
    state = RESETTING; /* Stop after this rotation. */
  }
  
  double x = radius_cm * cos(theta);
  double z = radius_cm * sin(theta);

#ifdef MODEL_POINT_CLOUD
  double inradius = POINT_CLOUD_POINT_SIZE_CM * sin(M_PI / 6);
  double halfEdgeLength = POINT_CLOUD_POINT_SIZE_CM * cos(M_PI / 6);

  emit("v " VERTEX_COMPONENT_FORMAT " " VERTEX_COMPONENT_FORMAT " " VERTEX_COMPONENT_FORMAT "\n", x - inradius, y - inradius, z - halfEdgeLength);
  emit("v " VERTEX_COMPONENT_FORMAT " " VERTEX_COMPONENT_FORMAT " " VERTEX_COMPONENT_FORMAT "\n", x - inradius, y - inradius, z + halfEdgeLength);
  emit("v " VERTEX_COMPONENT_FORMAT " " VERTEX_COMPONENT_FORMAT " " VERTEX_COMPONENT_FORMAT "\n", x + POINT_CLOUD_POINT_SIZE_CM, y - inradius, z);
  emit("v " VERTEX_COMPONENT_FORMAT " " VERTEX_COMPONENT_FORMAT " " VERTEX_COMPONENT_FORMAT "\n", x, y + POINT_CLOUD_POINT_SIZE_CM, z);

  int firstNewVertex = 1 /* The vertex numbers start at 1. */
                     + 4 /* The number of vertices per tetrahedron. */
                     * (
                       TABLE_STOPS_PER_ROTATION
                       * numberOfCompletedTowerStops
                       + numberOfVisitedTableStopsInThisRotation
                       - 1 /* We already incremented this above. */
                     );

  /* Floor. */
  emit("f %d %d %d\n", firstNewVertex, firstNewVertex + 1, firstNewVertex + 2);

  /* Upright sides. */
  emit("f %d %d %d\n", firstNewVertex, firstNewVertex + 1, firstNewVertex + 3);
  emit("f %d %d %d\n", firstNewVertex, firstNewVertex + 2, firstNewVertex + 3);
  
  /* Slanted side. */
  emit("f %d %d %d\n", firstNewVertex + 1, firstNewVertex + 2, firstNewVertex + 3);
#else
  emit("v " VERTEX_COMPONENT_FORMAT " " VERTEX_COMPONENT_FORMAT " " VERTEX_COMPONENT_FORMAT "\n", x, y, z);
#endif
}

void rotateThenEmitFaces()
{
  /* Rotate. */

  numberOfVisitedTableStopsInThisRotation = 0;
  for (int s = 0; s < TABLE_STOPS_PER_ROTATION; ++s)
    advanceTableThenEmitVertex();

  if (numberOfCompletedTowerStops < 1) /* If this is the first disk we can't form any faces yet. */
    return;

#ifndef MODEL_POINT_CLOUD
  /* Emit faces. */

  int firstUpperVertex = 1 + TABLE_STOPS_PER_ROTATION * numberOfCompletedTowerStops;
  int firstLowerVertex = firstUpperVertex - TABLE_STOPS_PER_ROTATION;

  for (int i = 0; i < TABLE_STOPS_PER_ROTATION; i++) {
    int iPlus1 = (i + 1) % TABLE_STOPS_PER_ROTATION;
    emit("f %d %d %d\n", firstLowerVertex + i, firstLowerVertex + iPlus1, firstUpperVertex + i);
    emit("f %d %d %d\n", firstUpperVertex + i, firstLowerVertex + iPlus1, firstUpperVertex + iPlus1);
  }
#endif
}

void scan()
{
  /* Allow the sensor to settle. */
  for (int i = 0; i < 5; ++i) {
    readDistance_mm();
  }

  openResultFile();

  towerMotorSetRotationDirection(false);
  numberOfCompletedTowerStops = 0;
  while (true) {
    rotateThenEmitFaces();
    if (numberOfCompletedTowerStops++ >= TOWER_LIMIT_STOPS || state != SCANNING)
      break;
    towerMotorAdvanceToNextStop();
  }

  flush();
  closeResultFile();

  state = RESETTING;
  showState();

  towerMotorSetRotationDirection(true);
  while (--numberOfCompletedTowerStops > 0) {
    towerMotorAdvanceToNextStop();
    delay(TOWER_LOWER_DELAY_PER_STOP_MS);
  }
}
