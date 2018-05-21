#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ground/bodies.h"
#include "space/planets.h"

#include "../libs/frozen/frozen.h"
//special thanks to the JSON library Frozen

void initPlanets()
{
  char *universe;
  universe = json_fread("universe.json");

  int len = strlen(universe);
  struct json_token t;
  for (int i = 0; json_scanf_array_elem(universe, len, ".planets", i, &t) > 0; i++)
  {
    pln braveNewWorld; //this function will return a brave new world indeed

    int braveX;
    int braveY;
    int braveSize;
    int braveThicc;
    char *braveName;

    int braveColors[3];
    /*
    trying to assign the results of json_scanf directly to the struct of the new planet behaved weirdly.
    So, for now, I made regular variables that I assign to and then the variables are copied to the planet structure.
    */

    json_scanf(t.ptr, t.len, "{name : %Q, x : %d, y : %d, size : %d, thickness : %d}", &braveName, &braveX, &braveY, &braveSize, &braveThicc);
    struct json_token colorT;
    for (int ci = 0; json_scanf_array_elem(t.ptr, t.len, ".color", ci, &colorT) > 0; ci++)
    {
      json_scanf(colorT.ptr, colorT.len, "%d", &braveColors[ci]);
      printf("Generated Color %d is %d\n", ci, braveColors[ci]);
      braveNewWorld.col[ci] = braveColors[ci];
    }

    braveNewWorld.location.x = braveX;
    braveNewWorld.location.y = braveY;
    braveNewWorld.name = braveName;
    braveNewWorld.size = braveSize;
    braveNewWorld.thickness = braveThicc;
    //TODO: put another array looper in here so that the program can read the color values for each planet from the file, too.
    planets[planetCount] = braveNewWorld;
    planetCount++;
  }
  free(universe);
}

body spoutStatic(FILE *inlevl, int argNumber, int typeOfThing)
{
 fgetc(inlevl);
 char arg = fgetc(inlevl);
 char line[80];
 int numbs[argNumber];
 int charcounter = 0;
 int numbcounter = 0;
 bool negate = false;
 while(arg != '\n' && arg != EOF)
 {
  printf("Argument is not a new line!\n");
  if (isdigit(arg))
  {
   line[charcounter] = arg; charcounter = charcounter + 1; printf("Argument is a number\n");
  }
  else if (arg == '-')
  {
   printf("Looks like a negative number!\n");
   negate = true;
  }
  else
  {
   printf("Argument is not a number. Possibly space, or a semi colon.\n");
   charcounter = 0;
   numbs[numbcounter] = atoi(line);
   if (negate){numbs[numbcounter] = -numbs[numbcounter]; negate = false;}
   numbcounter = numbcounter + 1;
   for(int i = 0; i < 80; i++){line[i] = 0;}
  }
  arg = fgetc(inlevl);
 }
 body thisNewBody;
 if (typeOfThing == TYPE_CHECKPOINT)
 {
  thisNewBody = newCheckpoint(numbs[0], numbs[1]);
 }
 else if(typeOfThing == TYPE_PLAYER)
 {
  thisNewBody = newBody(numbs[0], numbs[1], numbs[2], numbs[3], true, 8, 4, true, ABIL_NONE);
 }
 else if(typeOfThing == TYPE_HURT)
 {
  thisNewBody = newHurt(numbs[0], numbs[1], numbs[2], numbs[3], false);
 }
 else if(typeOfThing == TYPE_HURT_VIS)
 {
  thisNewBody = newHurt(numbs[0], numbs[1], numbs[2], numbs[3], true);
 }
 else
 {
  thisNewBody = newStatic(numbs[0], numbs[1], numbs[2], numbs[3]);
 }
 return thisNewBody;
}
