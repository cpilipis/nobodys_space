//include EVERYTHING
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h> //two pieces of the allegro library I need
enum mode{MODE_SPACE, MODE_GROUND};
int gamemode = MODE_SPACE;
bool redraw = false;
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h> //default C libraries I need
#include "controls.h"
#include "ground.c"
#include "space.c"

//enumerators for the key codes

int main(int argc, char **argv)
{
  for(int i = 1; i < argc; i++)
  {
    if (strcmp("-res", argv[i]) == 0)
    {
      SCREEN_W = atoi(argv[i+1]);
      SCREEN_H = atoi(argv[i+2]);
    }
  }
  srand(time(NULL));
  ALLEGRO_DISPLAY *display = NULL;
  ALLEGRO_EVENT_QUEUE *event_queue = NULL;
  ALLEGRO_TIMER *timer = NULL; //stuff we'll initialize
  bool key[5] = {false, false, false, false, false};
  bool redraw = true;
  bool doexit = false;
  //allegro initialization stuff
  if(!al_init())
  {
    fprintf(stderr, "failed to initialize allegro!\n");
    return -1;
  }

  if(!al_install_keyboard())
  {
    fprintf(stderr, "failed to initialize the keyboard!\n");
    return -1;
  }

  timer = al_create_timer(1.0 / FPS);
  if(!timer)
  {
    fprintf(stderr, "failed to create timer!\n");
    return -1;
  }

  display = al_create_display(SCREEN_W, SCREEN_H);
  if(!display) {
    fprintf(stderr, "failed to create display!\n");
    al_destroy_timer(timer);
    return -1;
  }
  bool prim = al_init_primitives_addon();
  if(!prim)
  {
    fprintf(stderr, "failed to init primitives!\n");
    al_destroy_timer(timer);
    return -1;
  }

  event_queue = al_create_event_queue();
  if(!event_queue)
  {
    fprintf(stderr, "failed to create event_queue!\n");
    al_shutdown_primitives_addon();
    al_destroy_display(display);
    al_destroy_timer(timer);
    return -1;
  }

  bool fonts = al_init_font_addon();
  if(!fonts)
  {
    fprintf(stderr, "failed to init fonts!\n");
    al_shutdown_primitives_addon();
    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);
    return -1;
  }
  bool funts = al_init_ttf_addon();
  if(!funts)
  {
    fprintf(stderr, "failed to init fonts!\n");
    al_shutdown_primitives_addon();
    al_shutdown_font_addon();
    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);
    return -1;
  }
  bool sounds = al_install_audio();
  if (!sounds)
  {
    fprintf(stderr, "failed to init sounds!\n");
    al_shutdown_primitives_addon();
    al_shutdown_font_addon();
    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);
    al_shutdown_ttf_addon();
    return -1;
  }
  bool soundCodec = al_init_acodec_addon();
  if (!soundCodec)
  {
    fprintf(stderr, "failed to init sounds!\n");
    al_shutdown_primitives_addon();
    al_shutdown_font_addon();
    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);
    al_uninstall_audio();
    al_shutdown_ttf_addon();
    return -1;
  }

  initHud();
  initSounds();
  spaceInit();

  al_register_event_source(event_queue, al_get_display_event_source(display));

  al_register_event_source(event_queue, al_get_timer_event_source(timer));

  al_register_event_source(event_queue, al_get_keyboard_event_source());

  al_clear_to_color(al_map_rgb(0,0,0));

  al_flip_display();

  al_start_timer(timer);
  bool gamePaused = false;
  //no more init stuff
  //draw the main menu
  menu();
  al_flip_display();
  bool start = false;
  while(!start)
  {
    //wait for a key press to start the game
    ALLEGRO_EVENT sev;
    al_wait_for_event(event_queue, &sev);
    if (sev.type == ALLEGRO_EVENT_KEY_DOWN)
    {
      start = true;
    }
    else if(sev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
    { //or, if the player closed the window at the pause menu, go straight to the end of the program
      doexit = true;
      start = true;
    }
  }
  while(!doexit)
  { //actual game loop
    ALLEGRO_EVENT ev;
    al_wait_for_event(event_queue, &ev); //create an event handler
    /*
    Allegro uses events for a number of things.
    Allegro triggers an event everytime a timer updates.
    We set this timer to 60 ticks a second earlier so we can have a consistent update speed for the game
    There's also events for key presses and even an event for the game being closed.
    We check these each loop of the while loop.
    */
    if(ev.type == ALLEGRO_EVENT_TIMER && !gamePaused) //when the game updates, which it does 60 times a second.
    {
      if (gamemode == MODE_SPACE)
      {
        space_update(key);
      }
      else if(gamemode == MODE_GROUND)
      {
        updateGround(key);
      }
    }
    else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) //when the player closes the game
    {
      doexit = true;
    }
    else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) //when a key is pressed
    {
      //check for keyboard input to feed to other functions
      //also, do things that we can't use the update loop for efficiently
      //such as playing the player's thrust sound, or pausing the game.
      switch(ev.keyboard.keycode)
      {
        case ALLEGRO_KEY_UP:
          key[KEY_UP] = true;
          if (gamemode == MODE_SPACE)
          {
          if(myShip.respawn < 300 && myShip.respawn > -1){playThrust();}
          else{stopThrustSound();}
          }
          break;

        case ALLEGRO_KEY_DOWN:
          key[KEY_DOWN] = true;
          break;

        case ALLEGRO_KEY_LEFT:
          key[KEY_LEFT] = true;
          break;

        case ALLEGRO_KEY_RIGHT:
          key[KEY_RIGHT] = true;
          break;

        case ALLEGRO_KEY_SPACE:
          key[KEY_SPACE] = true;
          break;
      }
    }
    else if(ev.type == ALLEGRO_EVENT_KEY_UP) //when a key is released
    {
      switch(ev.keyboard.keycode)
      {
        case ALLEGRO_KEY_UP:
          key[KEY_UP] = false;
          stopThrustSound();
          break;

        case ALLEGRO_KEY_DOWN:
          key[KEY_DOWN] = false;
          break;

        case ALLEGRO_KEY_LEFT:
          key[KEY_LEFT] = false;
          break;

        case ALLEGRO_KEY_RIGHT:
          key[KEY_RIGHT] = false;
          break;

        case ALLEGRO_KEY_ESCAPE:
          if (gamePaused){gamePaused = false;}
          else
          {
            gamePaused = true;
            drawPaused();
            al_flip_display();
          }
          break;

        case ALLEGRO_KEY_SPACE:
          key[KEY_SPACE] = false;
          break;
      }
    }

    if(redraw && al_is_event_queue_empty(event_queue))
    {
      if (gamemode == MODE_SPACE)
      {
        spaceDraw();
      }
      else if (gamemode == MODE_GROUND)
      {
        drawGround();
      }
    }
  }
  //destroy a bunch of things
  al_destroy_timer(timer);
  al_destroy_display(display);
  al_destroy_event_queue(event_queue);
  al_shutdown_primitives_addon();
  al_uninstall_audio();//Destroy EVERYTHING
  killHud();
  return 0;
}
