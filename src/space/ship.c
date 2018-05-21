#include "coor.h"
#include "asteroid.h"
#include "../drawing.h"
#include "sound.h"
#include "particle.h"
#include <allegro5/allegro.h>

//This type stores everything we need in a ship.
typedef struct ship
{
  double dir; //Dir is in radians. We will use it to do math on the player's direction to do stuff with velocity
  int score;
  int lives;
  int respawn; //respawn timer after the player is killed
  coor location;
  coor velocity;
  int oneUpScore; //How many points the player must earn to get a one-up
  float power; //how fast this player flies
  bool canFire; //Can the player fire the weapon in this tick?
  //This variable will be used to make sure the player can't hold the space key to spam lasers.
} ship;

ship newShip()
{ //create a basic player
  ship myShip;
  myShip.location.x = SCREEN_W/2;
  myShip.location.y = SCREEN_H/2;
  myShip.velocity.x = 0;
  myShip.velocity.y = 0; //go to the middle of the screen stood completely still
  myShip.dir = PI/2; //face completely up
  myShip.lives = 3; //start with 3 respawns
  myShip.respawn = 360; //Set the respawn timer to 6 seconds so that we have a sec to see where the asteroids are and then 5 seconds of invulnerability to react.
  myShip.power = .05; //How fast the player accelerates
  myShip.score = 0; //start with 0 score
  myShip.oneUpScore = 10000; //every 10000 points, the player gets a new life
  myShip.canFire = true; //The player can fire immediately after spawning in
  return myShip;
}

void killShip(ship * p)
{ //A function to kill the player when we know the player is touching an asteroid or something
  printf("Looks like a ship has died!");
  playExplosion(); //Play an explosion sound
  if (p->lives > 0)
  { //If the player has lives left
    printf(" Respawning...\n");
    p->lives--; //take a life
    p->respawn = 600; //600 ticks at 60 ticks a second would be 10 seconds
    //keep in mind that the player comes back at 300 ticks left and can fly around without limitation
    //but is invulnerable until p->respawn = 0
    //so, the player respawns with 5 seconds of spawn protection
    p->location.x = SCREEN_W/2;
    p->location.y = SCREEN_H/2; //put the player in the middle of the screen
    p->velocity.x = 0;
    p->velocity.y = 0; //stop the player's movement
    p->dir = PI/2;
  }
  else
  {
    p->lives--;
    printf(" No lives left... \n");
    p->respawn = -1; //The player never respawns if this value is less than 0
  }
}

bool shipVsAst(ship * p, ast * a, bool checkOnly)
{ //Check if a player is inside an asteroid
  coor pVerts[6]; //Shorthand for Player Veriticies
  //We'll get all the corners of the player's ship first
  //then we'll get the virtices between them
  //because if we don't small asteroids will be able to fly through the middle of the player
  //without harming the player.
  pVerts[0].x = cos(p->dir) * 22 + p->location.x;
  pVerts[0].y = sin(p->dir) * 22 + p->location.y;
  pVerts[1].x = cos(p->dir + PI/2) * 8 + p->location.x;
  pVerts[1].y = sin(p->dir + PI/2) * 8 + p->location.y;
  pVerts[2].x = cos(p->dir + PI/2) * 8 + p->location.x;
  pVerts[2].y = sin(p->dir + PI/2) * 8 + p->location.y;
  pVerts[3] = midpoint(pVerts[0], pVerts[1]);
  pVerts[4] = midpoint(pVerts[1], pVerts[2]);
  pVerts[5] = midpoint(pVerts[0], pVerts[2]);
  bool asteroidHit = false;
  for (int i = 0; i < 6; i++)
  { //for each collision point
    if(isInRadius(pVerts[i], a->location, a->size) && a->size != 0)
    { //if this vertice is actually in the asteroid
      asteroidHit = true;
      if(!checkOnly){particleExplosion(pVerts[i], 255, 165, 0);}
      break;
    }
  }
  if (asteroidHit && !checkOnly)
  { //If the player hit the asteroid and this function was not just being called as a collision test rather than a collision check
    //Sometimes I call this to check if an asteroid being spawned would spawn kill the player, and thus I set "Check Only" to true
    //That way I can simply check whether a player would die instead of having it happen.
    printf("Player was hit by an asteroid at (%f, %f), with a size of %d\n",a->location.x, a->location.y, a->size);
    killShip(p);
    return true;
  }else if (asteroidHit)
  {
    return true;
  }
  return false;
}
