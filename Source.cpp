#include <stdio.h>
#include <allegro5/allegro.h>
#include <iostream>
using namespace std;

//collision detection function
int playerCollidedWithGround(int y);
//this isn't working yet :'(
int playerCollidedWithPlatform(int PlayerX, int PlayerY, int PlayerWidth, int PlayerHeight, int PlatformX1, int PlatformX2, int PlatformY);

int main()
{
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_BITMAP *square = NULL;
	ALLEGRO_BITMAP *Plat1 = NULL;

	float square_x = 50;
	float square_y = 50;

	float Plat1x = 100;
	float Plat1y = 300;

	// velocity (in x and y, which means the speed we're going in each orthogonal direction)
	float dx = 0;
	float dy = 0;

	// to keep track if the player can jump again so he doesn't perform another jump midair
	bool mayJumpAgain = true;

	// is true if the player is on the ground
	bool isOnSolidGround = false;

	//here's our key states. they're all starting as "false" because nothing has been pressed yet.
	//the first slot represents "up", then "down", "left" and "right"
	bool key[4] = { false, false, false, false };

	//don't redraw until an event happens
	bool redraw = true;

	//this controls our game loop
	bool doexit = false;

	al_init();

	al_install_keyboard();

	timer = al_create_timer(.02);

	display = al_create_display(640, 480);

	square = al_create_bitmap(32, 32);


	al_set_target_bitmap(square);

	al_clear_to_color(al_map_rgb(255, 3, 55));

	Plat1 = al_create_bitmap(80, 32);

	al_set_target_bitmap(Plat1);

	al_clear_to_color(al_map_rgb(255, 20, 80));

	al_set_target_bitmap(al_get_backbuffer(display));

	event_queue = al_create_event_queue();

	al_register_event_source(event_queue, al_get_display_event_source(display));

	al_register_event_source(event_queue, al_get_timer_event_source(timer));

	al_register_event_source(event_queue, al_get_keyboard_event_source());

	al_clear_to_color(al_map_rgb(0, 0, 0));

	al_flip_display();

	al_start_timer(timer);

	//so the game loop is set to act on "ticks" of the timer OR keyboard presses 
	//OR the mouse closing the display
	while (!doexit)
	{
		if (isOnSolidGround)
			cout << "on solid ground" << endl;

		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		//////////////////////////////////////////////////////////////////////////////////////////////////
		//here's the movement algorithm

		if (ev.type == ALLEGRO_EVENT_TIMER) {

			 //if the left button is pressed 
			 //move the box left by 3 pixels
			if (key[2]) {
				dx = -3;
			}

			//if the right button is pressed 
			//move the box right by 3 pixels
			if (key[3]) {
				dx = 3;
			}

			//gravity
			//if we're not on solid ground, we should be falling (remember, positive y is "down")
			if (!isOnSolidGround) {
				dy += .5;
			}

			//put a cap on how fast you can fall
			if (dy > 5)
				dy = 5;

			//here's where your box's coordinates are acutally updated
			square_x += dx;
			square_y += dy;

			//here's my check to stop my square from walking off the window
			if (square_x > 640 - 30)
				square_x = 640 - 30;
			if (square_x < 0)
				square_x = 0;

			//this is basically a collision detection function
			//if we've collided with the bottom of the screen, change the velocity to 0, flip the flag
			if (playerCollidedWithGround(square_y)) {
				square_y = 480-30;
				dy = 0;
				isOnSolidGround = true;
			}
			else
				isOnSolidGround = false;

			//if (isOnSolidGround) {
				//friction: slow down by 10% each game loop
				//this is also the "stopping" mechanism- eventually it slows to *basically* zero
				dx *= .20;
				//if the up button is pressed
				if (key[0]) {
					if (mayJumpAgain) {
						//make your y direction go "up" by 10 pixels
						dy = -10;
						cout << "jumping" << endl;
						//no double jumping
						//mayJumpAgain = false;
					}
				}
				else
					mayJumpAgain = true;
			//}//end if on solid ground
			//redraw at every tick of the timer
			redraw = true;
		}
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		}
		//////////////////////////////////////////////////////////////////////////////////////////////////////
		//here's the algorithm that turns key presses into events
		//a "key down" event is when a key is pushed
		//while a "key up" event is when a key is released

		//has something been pressed on the keyboard?
		else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {

			//"keycode" holds all the different keys on the keyboard
			switch (ev.keyboard.keycode) {

				//if the up key has been pressed
			case ALLEGRO_KEY_UP:
				key[0] = true;
				break;

				//if the down key has been pressed
			case ALLEGRO_KEY_DOWN:
				key[1] = true;
				break;

				//if the left key has been pressed
			case ALLEGRO_KEY_LEFT:
				key[2] = true;
				break;

				//if the right key has been pressed
			case ALLEGRO_KEY_RIGHT:
				key[3] = true;
				break;
			}
		}
		//has something been released on the keyboard?
		else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
			switch (ev.keyboard.keycode) {
			case ALLEGRO_KEY_UP:
				key[0] = false;
				break;

			case ALLEGRO_KEY_DOWN:
				key[1] = false;
				break;

			case ALLEGRO_KEY_LEFT:
				key[2] = false;
				break;

			case ALLEGRO_KEY_RIGHT:
				key[3] = false;
				break;

				//kill the program if someone presses escape
			case ALLEGRO_KEY_ESCAPE:
				doexit = true;
				break;
			}
		}

		//if the clock ticked but no other events happened, don't bother redrawing
		if (redraw && al_is_event_queue_empty(event_queue)) {
			redraw = false;

			//paint black over the old screen, so the old square dissapears
			al_clear_to_color(al_map_rgb(0, 0, 0));

			//the algorithm above just changes the x and y coordinates
			//here's where the bitmap is actually drawn somewhere else
			al_draw_bitmap(square, square_x, square_y, 0);

			al_draw_bitmap(Plat1, Plat1x, Plat1y, 0);
			al_flip_display();
		}
	}

	al_destroy_bitmap(square);
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	return 0;
}

int playerCollidedWithGround(int y)
{
	if (y > 480 - 30) {
		//  collision
		cout << "collision!" << endl;
		return 1;
	}

	else
		// no collision
		return 0;
}

int playerCollidedWithPlatform(int PlayerX, int PlayerY, int PlayerWidth, int PlayerHeight, int PlatformX1, int PlatformX2, int PlatformY)
{
	if ((PlayerY + PlayerHeight < PlatformY) && (PlayerY + PlayerHeight + 3 > PlatformY) && (PlayerX + PlayerWidth < PlatformX1) && (PlayerX > PlatformX2))

	{//  collision
		cout << "collision!" << endl;
		return 1;
	}
	else
		// no collision
		return 0;
}