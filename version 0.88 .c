
/*		 TO DO LIST:
		1. structs
		2. WIN PATH
		target for saturday ->
		*finish snake tutorial
		*add car structs
		*add car movements
		*colision
		*6 random cars as struct, finish tutorials
		* half delay off on menus and screens, on in the game
		* vertical maps for left and right borders.
		* add lives
*/
#include <curses.h>
#include <cstdlib>
#include <time.h>
#define CAR_NUMBER 13
#define TIMER 300 //time for game
using namespace std;

//function that makes car "wait"/ modify their speed. Might be working not as intended
void sleep(int tenth)
{
	clock_t start = clock();  // clock_t = clock ticks
	clock_t end = start + tenth * CLOCKS_PER_SEC / 10;  // start + tenth * (clock per second)/10 [tenth of a second]

	while (clock() < end)
	{
		//wait...
	}
}

void handleMove(WINDOW* gamewin, int* yPos, int* xPos, int yMax, int xMax, char frog, int* waterReached, int* points, int* moves, char* verticalMap, int*intervalPassed)
{
	char move;
	move = getch();


	if (verticalMap[*yPos] == '=')
	{
		mvwaddch(gamewin, *yPos, *xPos, '=');
	}
	else
	{
		mvwaddch(gamewin, *yPos, *xPos, ' ');
	}
	if (move == 'w')
	{
		//function for checking whats on the board.
		(*moves)++;
		(*yPos)--;
		if (*yPos == 1)
		{
			(*waterReached)++;
		}
	}
	else if (move == 'a')
	{
		(*moves)++;
		(*xPos)--;
		if (*xPos < 1)
		{
			*xPos = 1;
		}
	}
	else if (move == 's')
	{
		(*moves)++;
		(*yPos)++;
		if (*yPos > yMax - 2)
		{
			*yPos = yMax - 2;
		}
	}
	else if (move == 'd')
	{
		(*moves)++;
		(*xPos)++;
		if (*xPos > xMax - 2)
		{
			*xPos = xMax - 2;
		}
	}
	wattron(gamewin, COLOR_PAIR(1));
	mvwaddch(gamewin, *yPos, *xPos, frog);
	wattroff(gamewin, COLOR_PAIR(1));
	wrefresh(gamewin);
}

//might need rework after cars will be added, move only frog and not the cars
void initializeGame(WINDOW* gamewin, int* yPos, int* xPos, int yMax, int xMax, char frog, char* verticalMap)
{
	wclear(gamewin);
	wrefresh(gamewin);
	box(gamewin, 0, 0);
	refresh();
	wrefresh(gamewin);
	*yPos = 23;
	*xPos = 25;
	wattron(gamewin, COLOR_PAIR(1));
	mvwprintw(gamewin, *yPos, *xPos, "F");
	wattroff(gamewin, COLOR_PAIR(1));
	mvwprintw(gamewin, 0, 2, "Jumping_frog");
	wattron(gamewin, COLOR_PAIR(2));
	for (int i = 1; i < xMax - 1; i++)
	{
		mvwprintw(gamewin, 1, i, "@");
	}
	wattroff(gamewin, COLOR_PAIR(2));
	for (int i = 2; i < yMax - 2; i++)
	{
		if (i % 4 == 2)
		{
			for (int j = 1; j < xMax - 1; j++)
			{
				mvwprintw(gamewin, i, j, "=");
				verticalMap[i] = '=';
			}
		}
	}

	wrefresh(gamewin);
	//printw("reach the water [@] to win!"); WAS DUPLIFIYING, NEED TO MOVE IT INTO DA OTHERT BOX 
}

//window with all statistics - [moves][points][time left][?lifes?]
void initializeStats(WINDOW* statbox, int points, int moves, clock_t start, int* gameOver)
{
	//clock_t currentTime = clock();
	//clock_t timer = currentTime + 300 * CLOCKS_PER_SEC;
	//int secondsLeft = (timer - start)/CLOCKS_PER_SEC;

	clock_t currentTime = clock();
	clock_t timer = currentTime - start;
	int secondsLeft = ((TIMER * CLOCKS_PER_SEC) - timer) / CLOCKS_PER_SEC; //unit of timer is also clocks_per_sec
	if (secondsLeft <= 0)
	{
		secondsLeft = 0;
		(*gameOver)++;
	}

	wclear(statbox);
	box(statbox, 0, 0);
	mvwprintw(statbox, 0, 2, "Player_stats");
	mvwprintw(statbox, 2, 2, "Points: %d", points);
	mvwprintw(statbox, 4, 2, "Total moves: %d", moves);
	mvwprintw(statbox, 6, 2, "Time left: %d", secondsLeft);
	wrefresh(statbox);
}

struct CAR {
	int speed;//car sleep [speed int]
	int y;// car y
	int x;// car x
	char automobile;// [later] color
	int leftRight;
	int initializeCounter; // counter for car to appear on the screen
	int carSpeed;
	int carSpeedCounter;
	// [later] lenght
	// [later] char
};

struct CAR CARS[CAR_NUMBER];

void moveCarsRight(WINDOW* gamewin, int yMax, int xMax, char* verticalMap)
{
	int randomValue;
	for (int i = 0; i < CAR_NUMBER; i++)
	{
		CARS[i].initializeCounter--;
		if (CARS[i].initializeCounter >= 0) //timer has passed, car now can get initialized, this doesnt lag. (hopefully) NEW FUNC
		{
			continue;
		}
		CARS[i].carSpeedCounter++;
		if (CARS[i].carSpeed >= CARS[i].carSpeedCounter) //NEW FUNC
		{
			continue;
		}
		randomValue = (rand() % 20) + 3;
		if (CARS[i].leftRight == 0) //0 = car moves left, 1 = car moves right
		{
			CARS[i].carSpeedCounter = 0;
				mvwaddch(gamewin, CARS[i].y, CARS[i].x, ' ');
				if (CARS[i].x == 48)
				{
					if (rand() % 2 == 1)
					{
						CARS[i].leftRight = 1;
					}
					else
					{
						CARS[i].x = 1;
						verticalMap[CARS[i].y] = ' ';//car position from the verticalMap is cleared

						while ((verticalMap[randomValue] == '=') || (verticalMap[randomValue] == 'C')) //checks if planned coordinate is safe zone or other car lane
						{
							randomValue = (rand() % 20) + 3;
						}
						verticalMap[randomValue] = 'C';
						CARS[i].y = randomValue; // car again appears onn the Right side, this time with different Y but still in bounds.
					}
				}
				else
				{
					CARS[i].x++;
				}
				wattron(gamewin, COLOR_PAIR(3));
				mvwaddch(gamewin, CARS[i].y, CARS[i].x, CARS[i].automobile);
				wattroff(gamewin, COLOR_PAIR(3));
		}
	}
	wrefresh(gamewin);
}

void moveCarsLeft(WINDOW* gamewin, int yMax, int xMax, char* verticalMap)
{
	int randomValue;
	for (int i = 0; i < CAR_NUMBER; i++)
	{
		CARS[i].initializeCounter--;
		if (CARS[i].initializeCounter >= 0)
		{
			continue;
		}
		CARS[i].carSpeedCounter++;
		if (CARS[i].carSpeed >= CARS[i].carSpeedCounter) //NEW FUNC
		{
			continue;
		}
			randomValue = (rand() % 20) + 3;
			if (CARS[i].leftRight == 1) //car is in rightMove mode
			{
				CARS[i].carSpeedCounter = 0;
				mvwaddch(gamewin, CARS[i].y, CARS[i].x, ' ');
				if (CARS[i].x == 1)
				{
					if (rand() % 2 == 1)  //50% chance
					{
						CARS[i].leftRight = 0;  //random chance for car to "bounce" instead of "wrapping" (changingleftmove to rightmove)
					}
					else
					{
						CARS[i].x = 48;                // car x is reseted
						verticalMap[CARS[i].y] = ' ';//car position from the map is cleared

						while ((verticalMap[randomValue] == '=') || (verticalMap[randomValue] == 'C')) //checks if planned coordinate is safe zone or other car lane
						{
							randomValue = (rand() % 20) + 3;
						}
						verticalMap[randomValue] = 'C';
						CARS[i].y = randomValue; // car again appears onn the left side, this time with different Y but still in bounds.

					}
				}
				else //not in border, just move
				{
					CARS[i].x--;
				}
				wattron(gamewin, COLOR_PAIR(3));
				mvwaddch(gamewin, CARS[i].y, CARS[i].x, CARS[i].automobile);
				wattroff(gamewin, COLOR_PAIR(3));
			}
			//sleep(CARS[i].speed); this lags program very much
		


	}
	wrefresh(gamewin);
}

void initializeCars(char* verticalMap)
{
	int value;

	for (int i = 0; i < CAR_NUMBER; i++)  //chwilowa zamiana na i bedace rand() % 8
	{
		value = (rand() % 20) + 3;
		CARS[i].leftRight = rand() % 2;
		if (CARS[i].leftRight == 0) //car will be moving left
		{

			while ((verticalMap[value] == '=') || (verticalMap[value] == 'C')) //checks if planned coordinate is safe zone or other car lane
			{
				value = (rand() % 20) + 3;
			}
			CARS[i].y = value;
			verticalMap[value] = 'C';
			CARS[i].x = 1;
			CARS[i].automobile = 'A';
			CARS[i].speed = (rand() % 5) + 1; //is it even working?
			CARS[i].initializeCounter = rand() % 100;
			//car random speeds
			CARS[i].carSpeed = (rand() % 3) * 3; //speed -> frames that car skip before moving
			CARS[i].carSpeedCounter = 0; // way to count how many frames car have skipped
		}
		else if ((CARS[i].leftRight == 1)) // car will be moving right
		{
			while ((verticalMap[value] == '=') || (verticalMap[value] == 'C'))
			{
				value = (rand() % 20) + 3;
			}
			CARS[i].y = value;
			verticalMap[value] = 'C';
			CARS[i].x = 48;
			CARS[i].automobile = 'A';
			CARS[i].speed = (rand() % 5) + 1;
			CARS[i].initializeCounter = rand() % 200;
			//car random speeds
			CARS[i].carSpeed = (rand() % 3) * 3; //speed -> frames that car skip before moving
			CARS[i].carSpeedCounter = 0; // way to count how many frames car have skipped
		}


	}
}

void checkColision(int yPos, int xPos, int* crash)
{
	for (int i = 0; i < CAR_NUMBER; i++)
	{
		if (CARS[i].x == xPos && CARS[i].y == yPos)
		{
			(*crash)++;
			break;
		}
	}

}

void initializeCurses()
{
	initscr();
	  // LETS CHECK IF SLEEP IS BETTER
	noecho();
	start_color();
	init_pair(1, COLOR_GREEN, COLOR_BLACK); //frog colors
	init_pair(2, COLOR_BLUE, COLOR_BLACK);  //water colors
	init_pair(3, COLOR_RED, COLOR_BLACK);
	cbreak();
	

}

void welcomeScreen()
{
	
	mvprintw(1, 5, "XXXX  X  X  X   X  XXX   X  X  X   XX");
	mvprintw(2, 5, "   X  X  X  XX XX  X  XX X  XX X  X  X");
	mvprintw(3, 5, "   X  X  X  X X X  X  X  X  XX X  X   ");
	mvprintw(4, 5, "   X  X  X  X   X  XXX   X  X XX  X XX ");
	mvprintw(5, 5, "   X  X  X  X   X  X     X  X XX  X  X");
	mvprintw(6, 5, "X  X  X  X  X   X  X     X  X  X  X  X");
	mvprintw(7, 5, " XX    XX   X   X  X     X  X  X   XX");
	mvprintw(10, 15,  "XXXX  XXX    XX    XX     XX    XX   X   X  XXXX");
	mvprintw(11, 15,  "X     X  X  X  X  X  X   X  X  X  X  XX XX  X   ");
	mvprintw(12, 15,  "X     X  X  X  X  X      X     X  X  X X X  XX     ");
	mvprintw(13, 15,  "XXXX  XXX   X  X  X XX   X XX  XXXX  X   X  XXXX");
	mvprintw(14, 15,  "X     XX    X  X  X  X   X  X  X  X  X   X  X     ");
	mvprintw(15, 15,  "X     X X   X  X  X  X   X  X  X  X  X   X  X     ");
	mvprintw(16, 15,  "X     X  X   XX    XX     XX   X  X  X   X  XXXX");
	mvprintw(20, 20, " @v@");
	mvprintw(21, 20, "(---)");
	mvprintw(25, 5, "press any key to continue . . .");
	getch();
	clear();
}

void handleGameOver()
{
	//wclear(gamewin);
	//mvwprintw(gamewin, 2, 2, "You won!!!! Press any key to restart :)");
	clear();
	welcomeScreen();
	sleep(10);


}

int main()
{
	srand(time(NULL)); // start randomizing with seed being time elapsed from 1970
	// PLAYER VARIABLES   --> STRUCT
	char frog = 'F';
	int yPos = 23;
	int xPos = 25; //starting coordinates
	int yMax; //bounds
	int xMax;
	int points = 0;
	int moves = 0;
	int waterReached = 0;		 //win condition reached
	int gameOver = 0;			 //time ended in future, end of lives
	int crash = 0;				 //collision
	// timer?? maybe in status...
	clock_t start = clock();
	int intervalPassed = 0;

	// CURSES START
	initializeCurses();
	welcomeScreen();
	//halfdelay(1);
	timeout(1);//1ms wait time for getch
	curs_set(0);
	WINDOW* gamewin = newwin(25, 50, 4, 3);
	WINDOW* statbox = newwin(10, 20, 4, 60);
	getmaxyx(gamewin, yMax, xMax);
	char* verticalMap = (char*)malloc((yMax - 2) * sizeof(char));
	printw("reach the water [@] to score 150 points, you lose a point every three moves!");
	refresh();
	initializeGame(gamewin, &yPos, &xPos, yMax, xMax, frog, verticalMap);
	initializeStats(statbox, points, moves, start, &gameOver);
	initializeCars(verticalMap);

	// MAIN GAME LOOP
	while (true)
	{
		if (waterReached == 1) //function
		{
			initializeGame(gamewin, &yPos, &xPos, yMax, xMax, frog, verticalMap);
			waterReached--;
			points += 150;
			initializeStats(statbox, points, moves, start, &gameOver);
		}
		if (crash == 1) // function
		{
			initializeGame(gamewin, &yPos, &xPos, yMax, xMax, frog, verticalMap);
			crash--;
			points -= 299;
			initializeStats(statbox, points, moves, start, &gameOver);
		}
		if (gameOver == 1)
		{
			handleGameOver();
			break;
		} 
		handleMove(gamewin, &yPos, &xPos, yMax, xMax, frog, &waterReached, &points, &moves, verticalMap, &intervalPassed);
		checkColision(yPos, xPos, &crash);
		moveCarsRight(gamewin, yMax, xMax, verticalMap);
		moveCarsLeft(gamewin, yMax, xMax, verticalMap);
		checkColision(yPos, xPos, &crash);
		initializeStats(statbox, points, moves, start, &gameOver);
	}

	getch();
	endwin();

}

//chtype currentField = mvwinch(gamewin, *yPos, (*xPos) + 1); // will not work on bounds... maybe make function isStreet?();
//char character = currentField & A_CHARTEXT;
//if (character == '=')
//{
//	mvwaddch(gamewin, *yPos, *xPos, '=');
//}
//else
//{
//	mvwaddch(gamewin, *yPos, *xPos, ' '); //clears current position to not leave "trace"
//}

//SEE IN TUTORIAL IF THIS IS AS USELESS AS IT SEEMS!!
