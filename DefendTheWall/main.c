#include <stdio.h>
#include <stdlib.h>
#include "myLib.h"
#include "text.h"
#include "images.h"

typedef struct
{
	int row;
	int col;
	int rdel;
	int cdel;
	int oldrow;
	int oldcol;
	int size;
	u16 color;
} TARGET;

typedef struct
{
	int row;
	int col;
	int cdel;
	int limit;
} PLANE;

typedef struct
{
	int row;
	int col;
	int cdel;
	int fired;
	int limit;
} CANNONBALL;

int main()
{
	REG_DISPCTL = MODE3 | BG2_ENABLE;

	drawImage(0, 0, STARTSCREEN_WIDTH, STARTSCREEN_HEIGHT, StartScreen);

	int seed=0;
	while(!KEY_DOWN_NOW(BUTTON_START)) //loop to keep it at start screen till we're ready to start
	{
		seed++;
	}
	srand(seed);

	int numPlanes = rand()%2 + 2;
	PLANE planes[numPlanes];

	for (int i = 0; i < numPlanes; i++) //initializing all the planes
	{
		planes[i].row = (rand()%130);
		planes[i].col = 240 - PLANE_WIDTH;
		planes[i].cdel = 0 - rand()%2 - 1;
		planes[i].limit = 0;
	}
	
	char printer[41];

	TARGET target; //initializing target
	target.row = 80;
	target.col = 120;
	target.rdel = 0;
	target.cdel = 0;
	target.size = 20;
	target.color = RED;

	CANNONBALL cb;
	cb.cdel = 5;
	cb.fired = 0;
	cb.limit = 240 - CANNONBALL_WIDTH;

	int health = 7;
	int score = 0;
	int winningScore = rand()%25 + 25;

	drawImage(0, 0, OCEAN_WIDTH, OCEAN_HEIGHT, ocean);
	drawImage(0, 0, WALL_WIDTH, WALL_HEIGHT, wall);

	sprintf(printer, "Score: %d", score);
	drawString(140, 180, printer, YELLOW);

	while(1)
	{
		if(KEY_DOWN_NOW(BUTTON_UP)) //These commands move the crosshairs when appropriate
		{
			if (target.row > 10)
			{
				target.rdel = target.rdel - 3;
			}
		}
		if(KEY_DOWN_NOW(BUTTON_DOWN))
		{
			if (target.row < 150)
			{
				target.rdel = target.rdel + 3;
			}
		}
		if(KEY_DOWN_NOW(BUTTON_LEFT))
		{
			if (target.col > 50)
			{
				target.cdel = target.cdel - 3;
			}
		}
		if(KEY_DOWN_NOW(BUTTON_RIGHT))
		{
			if (target.col < 230)
			{
				target.cdel = target.cdel + 3;
			}
		}
		if(KEY_DOWN_NOW(BUTTON_B)) //firing cannonball
		{
			if (!cb.fired)
			{
				cb.row = target.row;
				cb.col = WALL_WIDTH;
				cb.fired = 1;
				cb.limit = target.col + 8 - CANNONBALL_WIDTH; //ensure the cannonball stops at the target
			}
		}

		if (health <= 0)
		{
			drawImage(0, 0, LOSESCREEN_WIDTH, LOSESCREEN_HEIGHT, LoseScreen);
			while(!KEY_DOWN_NOW(BUTTON_SELECT)) //reset the game on press
			{
				seed++;
			}
			srand(seed);
			health = 7;
			drawImage(0, 0, OCEAN_WIDTH, OCEAN_HEIGHT, ocean);
			drawImage(0, 0, WALL_WIDTH, WALL_HEIGHT, wall);
		}

		if (score == winningScore)
		{
			drawImage(0, 0, VICTORYSCREEN_WIDTH, VICTORYSCREEN_HEIGHT, VictoryScreen);
			while(!KEY_DOWN_NOW(BUTTON_SELECT)) //reset the game on press
			{
				seed++;
			}
			srand(seed);
			health = 7;
			drawImage(0, 0, OCEAN_WIDTH, OCEAN_HEIGHT, ocean);
			drawImage(0, 0, WALL_WIDTH, WALL_HEIGHT, wall);
		}

		if (KEY_DOWN_NOW(BUTTON_SELECT)) //reset the game on press
		{
			drawImage(0, 0, STARTSCREEN_WIDTH, STARTSCREEN_HEIGHT, StartScreen);
	
			int seed=0;
			while(!KEY_DOWN_NOW(BUTTON_START))
			{
				seed++;
			}
			
			srand(seed);
			health = 7;
			score = 0;
			drawImage(0, 0, OCEAN_WIDTH, OCEAN_HEIGHT, ocean);
			drawImage(0, 0, WALL_WIDTH, WALL_HEIGHT, wall);
		}
	
		waitForVblank();

		//repair just affected background so tearing is eliminated
		repairBackground(target.row - 10, target.col - 10, target.size, target.size, ocean);
		target.row = target.row + target.rdel;
		target.col = target.col + target.cdel;
		target.rdel = 0;
		target.cdel = 0;
		drawTarget(target.row, target.col, target.size, target.color);

		for (int i = 0; i < numPlanes; i++)
		{
			repairBackground(planes[i].row, planes[i].col, PLANE_HEIGHT, PLANE_WIDTH, ocean);
			planes[i].col = planes[i].col + planes[i].cdel;

			//if the cannonball has hit the plane, destroy it
			if (cb.fired && planes[i].col < cb.col && cb.row > planes[i].row - 5
				&& cb.row + CANNONBALL_HEIGHT < planes[i].row + PLANE_HEIGHT + 5) 
			{
				planes[i].limit = 240;
				score++;
			}
			//if the plane hasn't met its limit, advance it forward
			if (planes[i].col > planes[i].limit)
			{
				drawImage(planes[i].row, planes[i].col, PLANE_HEIGHT, PLANE_WIDTH, plane);
			} else {
				//if the plane has hit the wall, decrement health
				if (planes[i].col < 20)
				{
					health--;
				}
				//create a "new" plane
				planes[i].row = (rand()%130);
				planes[i].col = 240 - PLANE_WIDTH;
				planes[i].cdel = 0 - rand()%2 - 1;
				planes[i].limit = 0;
			}
		}

		if (cb.fired) //balancing mechanism to only allow one cannonball on the screen at a time
		{
			if (cb.col < WALL_WIDTH)
			{
				repairBackground(cb.row, cb.col, CANNONBALL_HEIGHT, CANNONBALL_WIDTH, wall);
			} else {
				repairBackground(cb.row, cb.col, CANNONBALL_HEIGHT, CANNONBALL_WIDTH, ocean);
			}

			cb.col = cb.col + cb.cdel;

			if (cb.col < cb.limit)
			{
				drawImage(cb.row, cb.col, CANNONBALL_HEIGHT, CANNONBALL_WIDTH, cannonball);
			} else {
				cb.col = WALL_WIDTH;
				cb.fired = 0;
			}
		}
		
		//update score and health
		repairBackground(140, 180, 20, 60, ocean);
		sprintf(printer, "Score: %d", score);
		drawString(140, 180, printer, YELLOW);
		sprintf(printer, "Health: %d", health);
		drawString(150, 180, printer, YELLOW);
	}

}

