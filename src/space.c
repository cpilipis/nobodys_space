#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h> //two pieces of the allegro library I need
#include "space/coor.h"
#include "space/ship.h"
#include "space/shot.h"
#include "space/HUD.h"
#include "controls.h"
#include "space/sound.h"
#include "space/planets.h"
#include "space/particle.h" //everything I have written and migrated to other files
#include <math.h>
#include <time.h>

void drawShip(ship * p)
{
  float pX = p->location.x - spaceScrollX;
  float pY = p->location.y - spaceScrollY;
  //plots three points from the player's location and direction to form a triangle to draw.
  coor front = {cos(p->dir) * 22 + pX, sin(p->dir) * 22 + pY};
  coor top = {cos(p->dir + PI/2) * 8 + pX, sin(p->dir + PI/2) * 8 + pY};
  coor bottom = {cos(p->dir + 3*PI/2) * 8 + pX, sin(p->dir + 3*PI/2) * 8 + pY};
  al_draw_triangle(top.x, top.y, bottom.x, bottom.y, front.x, front.y, al_map_rgb(255, 255, 255), 0);
}

/*
void drawShot(bul * b)
{
  //Plot a point of where the laser came from last frame to provide a trailing laser effect to draw a line with
  float laserX = b->location.x;
  float laserMiddleX = b->location.x - b->velocity.x;
  float laserY = b->location.y;
  float laserMiddleY = b->location.y - b->velocity.y;
  al_draw_line(laserX, laserY, laserMiddleX, laserMiddleY, al_map_rgb(255, 100, 100), 0);
}*/
void drawParticle(part * p)
{  //draws a pixel where a particle is
  al_draw_circle(p->location.x - spaceScrollX, p->location.y - spaceScrollY, 0.5, al_map_rgb(p->color[0], p->color[1], p->color[2]), 0);
}
/*void drawAsteroid(ast * a)
{ //draws an asteroid based on it's location and size
  al_draw_circle(a->location.x, a->location.y, a->size, al_map_rgb(255, 255, 255), 0);
}*/

ship myShip;
bul shots[25];
const int maxAst = 100;
int particleTick;
int countShots;
bool redrawSpace;

void spaceInit()
{
  myShip = newShip();
  int particleTick = 0; //This will be used to time the spawning of particles from the player's thruster
  int countShots = 0;
  redrawSpace = true;
  initPlanets();
}


void boundPlayer(ship * p) //this function handles the player going out of bounds.
{ //Scroll the screen based on the player's location
  if (p->location.x > SCREEN_W + spaceScrollX- 400 && p->velocity.x > 0){spaceScrollX += p->velocity.x;}
  if (p->location.x < spaceScrollX + 400 && p->velocity.x < 0){spaceScrollX += p->velocity.x;}
  if (p->location.y > spaceScrollY + SCREEN_H - 300 && p->velocity.y > 0){spaceScrollY += p->velocity.y;}
  if (p->location.y < spaceScrollY + 300 && p->velocity.y < 0){spaceScrollY += p->velocity.y;}
}

void drawPlanets()
{
  for (int i = 0; i < planetCount; i++)
  {
    pln * p = &planets[i];
    float pX = p->location.x - spaceScrollX;
    float pY = p->location.y - spaceScrollY;
    al_draw_circle(pX, pY, p->size, al_map_rgb(p->col[0], p->col[1], p->col[2]), p->thickness);
  }
}

void space_update(bool key[5])
{
  particleTick++;
  if (myShip.respawn < 300 && myShip.respawn >= 0) //If the player respawned 5 seconds ago or longer
  { //get player input and update the player's position
    if (key[KEY_LEFT]){myShip.dir -= PI/30;}
    else if(key[KEY_RIGHT]){myShip.dir += PI/30;} //code to turn the player around on keypress
    if (key[KEY_UP])
    { //accelerate the player based on direction
      printf("%f\n", myShip.power);
      coor newVel = {cos(myShip.dir) * myShip.power, sin(myShip.dir) * myShip.power};
      myShip.velocity.x += newVel.x;
      myShip.velocity.y += newVel.y;
      //create firey particles to trail behind the player
      float opDir = myShip.dir + PI + ((float)(rand() % 100)/100 * PI/2 - PI/4);
      coor opVel;
      opVel.x = cos(opDir);
      opVel.y = sin(opDir);
      newParticle(myShip.location, opVel, 255, 100, 0);
    }
    if (key[KEY_DOWN])
    {
      char landName[15];
      bool planetLanded = false;
      for (int i = 0; i < planetCount; i++)
      {
        if (isInRadius(myShip.location, planets[i].location, planets[i].size))
        {
          printf("The ship is ready to land on planet %s\n", planets[i].name);
          strcpy(landName, planets[i].name);
          planetLanded = true;
          break;
        }
      }
      if (planetLanded)
      {
        loadPlanet(landName);
        printf("Landing at planet %s!\n", landName);
        gamemode = MODE_GROUND;
      }
    }
    myShip.location.x += myShip.velocity.x;
    myShip.location.y += myShip.velocity.y;
    boundPlayer(&myShip); //move the player across the screen and loop the player around if necessary
    if(myShip.respawn != 0){myShip.respawn--;} //If the player isn't done respawning, keep going
  }
  else
  {
    myShip.respawn--;
    stopThrustSound();
    //If the player is dead, stop any sounds from the player's rocket thrust.
  }
  //update shots
  //oh, and collide shots with asteroids while we're at it, using our table of live asteroids
  /*int deadIndex = 0;
  for (int i = 0; i < countShots; i++)
  {
    if(shots[i].age > 0)
    {
      shots[i] = updateShot(&shots[i]);
      //If the shot's not dead, update it
    }
    else
    {
      continue;
      //If this shot is dead, skip all the stuff about colliding it with an asteroid
    }
    for (int a = 0; a < liveCount; a++)
    {
      ast maybeDeadAst = checkAst(liveAst[a], &shots[i]); //store a new asteroid out of a collision check between the shot and this one
      //it may be dead, or it may be alive. Let's check
      if (maybeDeadAst.size != 0)
      { //if this new asteroid really did collide with a bullet and split up into two
        *deadAst[deadIndex] = maybeDeadAst;
        deadIndex++;
      }
      if (shots[i].age == 0){break;} //The shot automatically ages to 0 when it hits an asteroid, so we break out of checking this shot with the other asteroids if it already hit one
    }
  }*/
  //update particles
  for(int i = 0; i < maxParticles; i++)
  {
    if(particles[i].timeout > 0){updateParticle(&particles[i]);}
  }
  redraw = true;
}

void spaceDraw()
{
  redrawSpace = false;
  al_clear_to_color(al_map_rgb(0,0,0)); //clear the screen

  drawPlanets();
  //If player is not dead, or player just respawned and is not at a certain tick.
  //Setting it so that some frames the player draws gives the player a flashing effect when invulnerable.
  if((myShip.respawn/10 % 2 != 0 && myShip.respawn < 300 && myShip.respawn > 0) || myShip.respawn == 0){drawShip(&myShip);}
  /*for (int i = 0; i < countShots; i++)
  {
    if(shots[i].age != 0){drawShot(&shots[i]);}
    //draw live shots
  }*/
  /*for (int i = 0; i < maxAst; i++)
  {
    if(asteroids[i].size > 0){drawAsteroid(&asteroids[i]);}
    //draw live asteroids
  }*/
  for(int i = 0; i < maxParticles; i++)
  {
    if(particles[i].timeout > 0){drawParticle(&particles[i]);}
    //draw live particles
  }
  al_flip_display(); //take all this stuff we've drawn and put it on the screen
}
