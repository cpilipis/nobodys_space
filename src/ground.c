#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>

#include "ground/bodies.h"
#include "drawing.h"
#include "ground/libbodies.h"
#include "controls.h"
#include "loading.c"

body statics[50];
int staticcount = 0;
body players[3];
int playerCount = 0;

void loadPlanet(char pName[15])
{
  FILE *levl;
  char ch;
  char fName[64] = "planets/";
  strcat(fName, pName);
  strcat(fName, ".txt");
  levl = fopen(fName, "r");
  if(levl == NULL)
  {
    printf("Please specify a filename for the level. For example, ./Shape example_level.txt should run the example level.\n");
    //exit(0);
    gamemode = MODE_SPACE;
    return;
  }
  ch = fgetc(levl);
  int staticLimit = 50;//Don't store more statics than this variable allows for

  //TODO: IMPLEMENT A SYSTEM THAT COUNTS THE STATIC OBJECTS IN A FILE
  //RATHER THAN GIVE THE STATICS VARIABLE AN ARBITARY LIMIT
  staticcount = 0;
  while (ch != EOF)
  {
    printf("Char is not EOF!\n");
    while(ch == ' ' || ch == '\n' || ch == ';'){ch = fgetc(levl); printf("Looks like we've got some dead space, or a stray semicolon.\n");}
    if (ch == 's')
    {
      printf("Let's add a static object.\n");
      statics[staticcount] = spoutStatic(levl, 4, TYPE_STATIC);
      staticcount++;
      ch = fgetc(levl);
    }
    if (ch == 'p')
    {
      printf("Let's add a player.\n");
      players[playerCount] = spoutStatic(levl, 4, TYPE_PLAYER);
      cameraFocusObject = &players[playerCount];
      playerCount++;
      ch = fgetc(levl);
    }
    if (ch == 'c')
    {
      printf("Let's add a checkpoint.\n");
      statics[staticcount] = spoutStatic(levl, 2, TYPE_CHECKPOINT);
      staticcount++;
      ch = fgetc(levl);
    }
    if (ch == 'h')
    {
      printf("Let's add a hurt trigger.\n");
      statics[staticcount] = spoutStatic(levl, 4, TYPE_HURT_VIS);
      staticcount++;
      ch = fgetc(levl);
    }
  }
  fclose(levl);
}

void updateGround(bool key[])
{
  for(int i = 0; i < playerCount; i++)
  {
    players[0] = updateBody(players[0], statics, staticcount, key);
  }
  if (key[KEY_SPACE])
  {
    for (int i = 0; i < 50; i++)
    {
      statics[i] = newStatic(-100, -100, -100, -100);
    }
    staticcount = 0;
    playerCount = 0;
    for (int i = 0; i < 3; i++)
    {
      players[i] = newStatic(-100, -100, -100, -100);
    }
    xscreen = 0;
    yscreen = 0;
    gamemode = MODE_SPACE;
  }
  if (cameraFocusObject != NULL) //this pertains to players but isn't in the for loop
  {//because we don't want to make the screen scroll for every player, just the first one.
   if(cameraFocusObject->x > SCREEN_W + xscreen - 400 && cameraFocusObject->xvel > 0){xscreen = xscreen + cameraFocusObject->xvel;}
   if(cameraFocusObject->x < xscreen + 400 && cameraFocusObject->xvel < 0){xscreen = xscreen + cameraFocusObject->xvel;}
   if(cameraFocusObject->y > SCREEN_H + yscreen - 300 && cameraFocusObject->yvel > 0){yscreen = yscreen + cameraFocusObject->yvel;}
   if(cameraFocusObject->y < yscreen + 300 && cameraFocusObject->yvel <= 0){yscreen = yscreen + cameraFocusObject->yvel;}
  }
  redraw = true;
}

 /*for(int i = 2; i < argc; i++)
 {
  if (strcmp("-res", argv[i]) == 0){SCREEN_W = atoi(argv[i+1]); SCREEN_H = atoi(argv[i+2]);}
  if (strcmp("-trippy", argv[i]) == 0){trippy = true;}
  if (strcmp("-ability", argv[i]) == 0)
  {
   if(strcmp(argv[i + 1], "morph") == 0){playerAbility = ABIL_MORPH;}
   if(strcmp(argv[i + 1], "boost") == 0){playerAbility = ABIL_BOOST;}
  }
  if (strcmp(argv[i], "-backgroundColor") == 0)
  {
   for (int n = 0; n < 3; n++)
   {
    backCols[n] = atoi(argv[i + 1 + n]);
   }
  }
  if (strcmp(argv[i], "-foregroundColor") == 0)
  {
   for (int n = 0; n < 3; n++)
   {
    foreCols[n] = atoi(argv[i + 1 + n]);
   }
  }
 }*/
void drawGround()
{
  al_clear_to_color(al_map_rgb(backCols[0], backCols[1], backCols[2]));
  int i;
  for (i = 0; i < playerCount; i++){drawBody(players[i]);}
  printf("Drew %d players", i);
  for(i = 0; i < staticcount; i++){drawBody(statics[i]);} //draw everything
  printf(" and %d statics this frame.\n", i);
  al_flip_display();
}
