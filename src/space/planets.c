#include "coor.h"
#include <allegro5/allegro_primitives.h>
#include <string.h>
#include <math.h>
#include <time.h>

float spaceScrollX = 0;
float spaceScrollY = 0;

typedef struct planet
{
  coor location;
  int size;
  int thickness;
  int col[3];
  char *name;
} pln;

pln planets[20] = {};
int planetCount = 0;

pln newPlanet(coor location, int size, int thickness, char name[])
{
  pln p;
  p.location = location;
  p.size = size;
  p.thickness = thickness;
  for (int c = 0; c < 2; c++)
  {
    p.col[c] = rand() % 255;
    srand(time(NULL));
  }
  printf("Creating %s planet at (%f, %f) with size %d\n", name, location.x, location.y, size);
  strcpy(p.name, name);
  return p;
}
