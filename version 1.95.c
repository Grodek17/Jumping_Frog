//Mateusz Grodecki 198385
/*		 TO DO LIST:
		*finish snake tutorial
		*colision "roszada bug"
		* add lives
		* car formula = gameheight = x  -> (x-5)-((x-5)/4)-1
*/
#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <curses.h>
#include <string.h>
#include <time.h>
using namespace std;

#define CAR_NUMBER 14 
#define TIMER 90						//time for game
#define JUMP_DELAY 150					//delay between jumps
#define CONFIG_NAME "frogconfig.txt"
#define SCORE_NAME "ranking.txt"
#define OBSTACLE_COUNT 30
#define NEUTRAL_CHANCE 7				// ~1/7 cars will be neutral
#define FRIENDLY_CHANCE 7
#define REQUEST_KEY 'e'

//=====DATA STRUCTURES================
typedef struct {
	char frog;
	int y;
	int x; 
	int startX;		//starting coordinates for stork
	int startY;
	int points;
	int moves;
	int water;	 //win condition reached
	int gameOver;		 //time ended / if addedend of lives
	int crash;			 //collision
	int taxiRequest;	//is taximode on for frog
	int taxi;			//number of car thats transporting the frog
	int interval;		//0 if interval passed, 1 if not
	char username[25]; //for scoreboard
	int taxiButton;
	int taxiNearby;
} PLAYER;

typedef struct {
	int speed;//car sleep [speed int]
	int y;
	int x;
	char automobile;
	int rightLeft;
	int initializeCounter; // counter for car to appear on the screen
	int carSpeed;
	int carSpeedCounter;
	int neutral;
	int friendly;
}CAR;

typedef struct {
	int game_width;
	int game_height;
	char car_character;
	int car_color;
	int game_speed;
} Config;

typedef struct {
	int y, x;
	char ROCK;
}OBSTACLE;

typedef struct {
	char name[25];
	int points;
}SCORE;

typedef struct {
	int y;
	int x;
	int intervalPassed;
	char stork;
}BIRD;


//============FUNCTIONS=====================================
void sleep(int ms)
{
	clock_t start = clock();  // clock_t = clock ticks
	clock_t end = start + ms * CLOCKS_PER_SEC / 1000;  // start + tenth * (clock per second)/10 [tenth of a second]

	while (clock() < end)
	{
		//wait...
	}
}

int checkRock(int y, int x, OBSTACLE* ROCK, PLAYER* FROG) // new
{
	for (int i = 0; i < OBSTACLE_COUNT; i++)
	{
		if (x == ROCK[i].x && y == ROCK[i].y)
		{
			return 1;
		}

	}
	FROG->taxiRequest = 0;   //frog gets out of taxi if wasn't blocked by an obstacle
	return 0;
}

void initPlayer(PLAYER* FROG, BIRD* STORK, Config* CONFIG)
{
	FROG->points = 0;
	FROG->moves = 0;
	FROG->water = 0;
	FROG->gameOver = 0;
	FROG->crash = 0;
	FROG->taxiRequest = 0;
	FROG->taxi = -1;
	FROG->interval = 0;
	FROG->taxiButton = 0;
	FROG->taxiNearby = 0;

	if (rand() % 2 == 1)
	{
		STORK->x = 1;
	}
	else
	{
		STORK->x = CONFIG->game_width - 2;
	}
	STORK->y = 3;
	STORK->intervalPassed = 0;
	STORK->stork = '*';
}

void checkRequest(WINDOW* game, PLAYER* FROG, CAR* CARS)
{
	//int taxi is needed for knowing which car will be our "taxi" - which CAR[taxi] coordinates will our player get
	int whichCar = -1;
	FROG->taxiNearby = 0;
	for (int i = 0; i < CAR_NUMBER; i++)						//checking if there is a friendly car on a player lane
	{
		if ((FROG->y == CARS[i].y) && (CARS[i].friendly == 1))
		{
			whichCar = i;
		}
	}
	if (whichCar == -1)
	{
		return;
	}
	if (CARS[whichCar].rightLeft == 0)							//checking if frog is close enough to make request
	{
		if (CARS[whichCar].x == (FROG->x - 1))
		{
			FROG->taxiNearby = 1;								//allows to activate the request in move function
		}
	}
	if (CARS[whichCar].rightLeft == 1)
	{
		if (CARS[whichCar].x == (FROG->x + 1))
		{
			FROG->taxiNearby = 1;	
		}
	}

	if (FROG->taxiButton)										//if request got activated in move function
	{
		FROG->taxiRequest = 1;
		FROG->taxi = whichCar;									
		mvwaddch(game, FROG->y, FROG->x, ' '); //frog "gets in" to the car -> it disappears from the screen
		FROG->taxiButton = 0;
	}
}

void reprint(WINDOW* game, PLAYER* FROG)
{
	wattron(game, COLOR_PAIR(1));
	mvwaddch(game, FROG->y, FROG->x, FROG->frog);
	wattroff(game, COLOR_PAIR(1));
}

void updateStorkPos(BIRD* STORK, PLAYER* FROG)
{
	if (STORK->y == FROG->startY && STORK->x == FROG->startX)
	{
		STORK->x = 1;
		STORK->y = 3;
	}
	else if ((FROG->x == STORK->x) && (FROG->y > STORK->y))
	{
		STORK->y++;
	}
	else if ((STORK->x < FROG->x) && (STORK->y < FROG->y))
	{
		STORK->y++;
		STORK->x++;
	}
	else if ((STORK->x < FROG->x) && (STORK->y == FROG->y))
	{
		STORK->x++;
	}
	else if ((STORK->x < FROG->x) && (STORK->y == FROG->y))
	{
		STORK->x++;
	}
	else if ((STORK->x < FROG->x) && (STORK->y > FROG->y))
	{
		STORK->x++;
		STORK->y--;
	}
	else if ((STORK->x == FROG->x) && (STORK->y > FROG->y))
	{
		STORK->y--;
	}
	else if ((STORK->x > FROG->x) && (STORK->y < FROG->y))
	{
		STORK->y++;
		STORK->x--;
	}
	else if ((STORK->x > FROG->x) && (STORK->y == FROG->y))
	{
		STORK->x--;
	}
	else if ((STORK->x > FROG->x) && (STORK->y < FROG->y))
	{
		STORK->y++;
		STORK->x--;
	}
}

void moveStork(WINDOW* game, BIRD* STORK, clock_t* sinterval, PLAYER* FROG, char* carMap, OBSTACLE* ROCK)
{
	if (STORK->intervalPassed == 1)
	{
		return;
	}
	int rock = 0;										//helper variable for checking if stork is on obstacle field
	for (int i = 0; i < OBSTACLE_COUNT; i++)
	{
		if ((STORK->x == ROCK[i].x) && (STORK->y == ROCK[i].y))
		{
			rock = 1;
		}
	}
	if (rock)
	{
		mvwaddch(game, STORK->y, STORK->x, '#');
	}
	else if (carMap[STORK->y] == '=')
	{
		mvwaddch(game, STORK->y, STORK->x, '-');
	}
	else if (STORK->y == FROG->startY && STORK->x == FROG->startX)
	{
		//do not clear the frog at spawn
	}
	else
	{
		mvwaddch(game, STORK->y, STORK->x, ' ');
	}
	updateStorkPos(STORK, FROG);

	mvwaddch(game, STORK->y, STORK->x, STORK->stork);
	if ((STORK->x == FROG->x) && (STORK->y == FROG->y))
	{
		FROG->crash = 1;
	}
	wrefresh(game);
	
	STORK->intervalPassed = 1;
	*sinterval = clock();						//time need to pass for stork to move again

}

void handleMove(WINDOW* game, PLAYER* FROG, char* carMap, clock_t* interval, OBSTACLE* ROCK, Config* CONFIG)

{
	if (FROG->interval == 1)
	{
		return;
	}
	char move = getch(); 
	if (move == ERR)
	{
		return;
	}


	if (carMap[FROG->y] == '=')
	{
		mvwaddch(game, FROG->y, FROG->x, '-');
	}
	else if (FROG->taxiRequest)
	{
		//leave the taxi on the screen!! (when getting out off the car)
	}
	else
	{
		mvwaddch(game, FROG->y, FROG->x, ' ');
	}


	int tempY = FROG->y;  //helper variables
	int tempX = FROG->x;
	switch (move)
	{
	case 'w':
		tempY--;
		if (tempY == 1)
		{
			FROG->water++;
		}
		break;

	case 'a':
		tempX--;
		if (tempX < 1)
		{
			tempX = 1;
		}
		break;

	case 's':
		tempY++;
		if (tempY > CONFIG->game_height - 2)
		{
			tempY = CONFIG->game_height - 2;
		}
		break;

	case 'd':
		tempX++;
		if (tempX > CONFIG->game_width - 2)
		{
			tempX = CONFIG->game_width - 2;
		}
		break;
	case REQUEST_KEY:
		if (FROG->taxiNearby)
		{
			FROG->taxiButton = 1;
		}
		break;
	}
	if (checkRock(tempY, tempX, ROCK, FROG)) //if there is an obstacle then frog coordinates aren't updated
	{
		reprint(game, FROG);
		return;
	}
	else // coordinates updated only if frog moved
	{
		FROG->x = tempX;
		FROG->y = tempY;
	}
	if (move != ERR)
	{
		FROG->moves++;
		*interval = clock();
		FROG->interval = 1;
		//taxirequest = 0 happens inside of checkrock(); 
	}
	reprint(game, FROG);
	wrefresh(game);
}

void initGame(WINDOW* game, PLAYER* FROG, char* carMap, OBSTACLE* ROCK, Config* CONFIG)
{
	noecho();
	timeout(0);
	curs_set(0);
	int rockCounter = 0;
	wclear(game);
	wrefresh(game);
	box(game, 0, 0);
	refresh();
	wrefresh(game);
	FROG->y = CONFIG->game_height - 2;
	FROG->x = CONFIG->game_width / 2;
	FROG->startY = CONFIG->game_height - 2;
	FROG->startX = CONFIG->game_width / 2;
	wattron(game, COLOR_PAIR(1));
	mvwaddch(game, FROG->y, FROG->x, FROG->frog);
	wattroff(game, COLOR_PAIR(1));
	mvprintw(1, 2, "reach the water [@] to score 150 points");
	if (CONFIG->game_width > 15)
	{
		mvwprintw(game, 0, 2, "Jumping_frog");
	}

	else
	{
		mvwprintw(game, 0, 2, "frg");
	}
	wattron(game, COLOR_PAIR(2));
	for (int i = 1; i < CONFIG->game_width - 1; i++)
	{
		mvwprintw(game, 1, i, "@");
	}
	wattroff(game, COLOR_PAIR(2));
	for (int i = 2; i < CONFIG->game_height - 2; i++)
	{
		if (i % 4 == 2)  // safe spot every 4 fields
		{
			for (int j = 1; j < CONFIG->game_width - 1; j++)
			{
				if (rand() % 7 == 0 && rockCounter < OBSTACLE_COUNT)  //random chance for rock to appear between car lanes
				{
					mvwprintw(game, i, j, "#");
					ROCK[rockCounter].x = j;
					ROCK[rockCounter].y = i;
					rockCounter++;
				}
				else
				{
					mvwprintw(game, i, j, "-");
					carMap[i] = '=';
				}

			}
		}
	}
	wrefresh(game);
}

void refreshGame(WINDOW* game, PLAYER* FROG, Config* CONFIG)
{
	if (FROG->crash == 1)
	{
		FROG->points -= 300;
		FROG->crash = 0;
		wattron(game, COLOR_PAIR(2));
		mvwprintw(game, FROG->y, FROG->x, " ");
		wattroff(game, COLOR_PAIR(2));
	}
	if (FROG->water == 1)
	{
		FROG->points += 500;
		FROG->water--;
		wattron(game, COLOR_PAIR(2));
		mvwprintw(game, FROG->y, FROG->x, "@");
		wattroff(game, COLOR_PAIR(2));
	}

	FROG->y = CONFIG->game_height - 2;
	FROG->x = CONFIG->game_width / 2;

	wattron(game, COLOR_PAIR(1));
	mvwprintw(game, FROG->y, FROG->x, "F");
	wattroff(game, COLOR_PAIR(1));

	refresh();
	wrefresh(game);
}

void initStats(WINDOW* statbox, PLAYER* FROG, clock_t start)
{
	clock_t currentTime = clock();
	clock_t timer = currentTime - start;
	int secondsLeft = ((TIMER * CLOCKS_PER_SEC) - timer) / CLOCKS_PER_SEC; //unit of timer is also clocks_per_sec
	if (secondsLeft <= 0)
	{
		secondsLeft = 0;
		FROG->gameOver++;
	}

	wclear(statbox);
	box(statbox, 0, 0);
	mvwprintw(statbox, 0, 2, "Player_stats");
	mvwprintw(statbox, 2, 2, "Points: %d", FROG->points);
	mvwprintw(statbox, 4, 2, "Total moves: %d", FROG->moves);
	mvwprintw(statbox, 6, 2, "Time left: %ds", secondsLeft);
	mvwprintw(statbox, 8, 2, "Mateusz Grodecki");
	mvwprintw(statbox, 9, 2, "198385");
	wrefresh(statbox);
}

int checkFrogProximity(int carNum, PLAYER* FROG, CAR* CARS)
{
	if (FROG->taxiRequest == 1)
	{
		return 0;					 //being in car means that car will not stop
	}
	if (CARS[carNum].rightLeft == 1) //0-right  1-left
	{
		if (CARS[carNum].y != FROG->y)
		{
			return 0;
		}
		if (FROG->x > CARS[carNum].x)
		{
			return 0;
		}
		else if (((CARS[carNum].x - FROG->x) < 3) && ((CARS[carNum].x - FROG->x) > 0))
		{
			return 1;
		}
	}


	if (CARS[carNum].rightLeft == 0) //same logic but for cars going right
	{
		if (CARS[carNum].y != FROG->y)
		{
			return 0;
		}
		if (FROG->x < CARS[carNum].x)
		{
			return 0;
		}
		else if (((FROG->x - CARS[carNum].x) < 3) && ((FROG->x - CARS[carNum].x) > 0))
		{
			return 1;
		}
	}
	return 0;
}

void printCar(WINDOW* game, int color, CAR* CARS, int i, BIRD* STORK)
{
	if (CARS[i].y == STORK->y && CARS[i].x == STORK->x) //does not print car if stork is "over" the car
	{
		return;
	}
	wattron(game, COLOR_PAIR(color));
	mvwaddch(game, CARS[i].y, CARS[i].x, CARS[i].automobile);
	wattroff(game, COLOR_PAIR(color));
}

void reInitCar(int i, CAR* CARS, char* carMap, Config* CONFIG)
{
	int randomValue = (rand() % (CONFIG->game_height - 4)) + 2;
	carMap[CARS[i].y] = ' ';
	while ((carMap[randomValue] == '=') || (carMap[randomValue] == 'C')) //checks if planned coordinate is safe zone or other car lane
	{
		randomValue = (rand() % (CONFIG->game_height - 4)) + 2;
	}
	carMap[randomValue] = 'C';
	CARS[i].y = randomValue; // car again appears on the other side, this time with different Y but still in bounds.
	CARS[i].initializeCounter = CARS[i].initializeCounter = rand() % 150;

	if (CARS[i].rightLeft == 0)
	{
		CARS[i].x = 1;
	}
	if (CARS[i].rightLeft == 1)
	{
		CARS[i].x = CONFIG->game_width - 2;                // car x is reseted
	}
}

void mvCarsRight(WINDOW* game, char* carMap, PLAYER* FROG, CAR* CARS, Config* CONFIG, int i, BIRD* STORK)
{


	CARS[i].initializeCounter--;
	if (CARS[i].initializeCounter >= 0) //timer has passed, car now can get initialized
	{
		return;
	}

	CARS[i].carSpeedCounter++;
	if (CARS[i].carSpeed >= CARS[i].carSpeedCounter) //car moves every x ammount of iterations of the loop
	{
		return;
	}

	CARS[i].carSpeedCounter = 0;

	if ((CARS[i].neutral == 1) || (CARS[i].friendly == 1))//certain cars stopping when frog is in front of them
	{
		if (checkFrogProximity(i, FROG, CARS))  
		{
			return;								//friendly and neutral cars stops
		}
	}
	if (CARS[i].y == STORK->y && CARS[i].x == STORK->x)
	{
		mvwaddch(game, CARS[i].y, CARS[i].x, '*');
	}
	else
	{
		mvwaddch(game, CARS[i].y, CARS[i].x, ' ');
	}
	if (rand() % 29 == 0)							//low chance for change of a speed
	{
		CARS[i].carSpeed = (rand() % 3) * 3;
	}
	if (CARS[i].x == CONFIG->game_width - 2)
	{
		if (rand() % 2 == 1)
		{
			CARS[i].rightLeft = 1;

		}
		else
		{ 
			reInitCar(i, CARS, carMap, CONFIG);
			return;
		}
	}
	else
	{
		CARS[i].x++;
	}
	if ((CARS[i].neutral == 1))
	{
		printCar(game, 6, CARS, i, STORK);
	}
	else if ((CARS[i].friendly == 1))
	{
		printCar(game, 5, CARS, i, STORK);
	}
	else
	{
		printCar(game, CONFIG->car_color, CARS, i, STORK);
	}


	refresh();
	wrefresh(game);
}

void mvCarsLeft(WINDOW* game, char* carMap, PLAYER* FROG, CAR* CARS, Config* CONFIG, int i, BIRD* STORK)
{

	CARS[i].initializeCounter--;
	if (CARS[i].initializeCounter >= 0)
	{
		return;
	}
	CARS[i].carSpeedCounter++;
	if (CARS[i].carSpeed >= CARS[i].carSpeedCounter)
	{
		return;
	}

	CARS[i].carSpeedCounter = 0;
	if ((CARS[i].neutral == 1) || (CARS[i].friendly == 1))
	{
		if (checkFrogProximity(i, FROG, CARS))
		{
			return;
		}
	}
	if (CARS[i].y == STORK->y && CARS[i].x == STORK->x)
	{
		mvwaddch(game, CARS[i].y, CARS[i].x, '*');
	}
	else
	{
		mvwaddch(game, CARS[i].y, CARS[i].x, ' ');
	}
	if (rand() % 29 == 0)
	{
		CARS[i].carSpeed = (rand() % 3) * 3;
	}
	if (CARS[i].x == 1)
	{
		if (rand() % 2 == 1)  //50% chance
		{
			CARS[i].rightLeft = 0;  //random chance for car to "bounce" instead of "wrapping" (changingleftmove to rightmove)

		}
		else
		{
			reInitCar(i, CARS, carMap, CONFIG);
			return;

		}
	}
	else //not in border, just move
	{
		CARS[i].x--;
	}

	if ((CARS[i].neutral == 1))
	{
		printCar(game, 6, CARS, i, STORK);
	}
	else if ((CARS[i].friendly == 1))
	{
		printCar(game, 5, CARS, i, STORK);
	}
	else
	{
		printCar(game, CONFIG->car_color, CARS, i, STORK);
	}


	refresh();
	wrefresh(game);
}

//updating the frog coordinnates while it is in the friendly car
void mvTaxi(WINDOW* game, PLAYER* FROG, CAR* CARS, Config* CONFIG)
{
	if ((FROG->taxiRequest) == 0)
	{
		return;
	}
	FROG->y = CARS[FROG->taxi].y;
	FROG->x = CARS[FROG->taxi].x;
	if (CARS[FROG->taxi].rightLeft == 1)
	{
		if (CARS[FROG->taxi].x == 1)
		{
			FROG->taxiRequest = 0;
			FROG->x = (CARS[FROG->taxi].x + 1);
			wattron(game, COLOR_PAIR(1));
			mvwaddch(game, FROG->y, FROG->x, FROG->frog);
			wattroff(game, COLOR_PAIR(1));
		}
	}
	if (CARS[FROG->taxi].rightLeft == 0)
	{
		if (CARS[FROG->taxi].x == (CONFIG->game_width - 2))
		{
			FROG->taxiRequest = 0;
			FROG->x = (CARS[FROG->taxi].x - 1);
			wattron(game, COLOR_PAIR(1));
			mvwaddch(game, FROG->y, FROG->x, FROG->frog);
			wattroff(game, COLOR_PAIR(1));
		}
	}
	refresh();
}

void mvCars(WINDOW* game, char* carMap, PLAYER* FROG, CAR* CARS, Config* CONFIG, BIRD* STORK)
{
	for (int i = 0; i < CAR_NUMBER; i++)
	{
		if (CARS[i].rightLeft == 0)							//0 = car moves left, 1 = car moves right
		{
			mvCarsRight(game, carMap, FROG, CARS, CONFIG, i, STORK);
		}
		if (CARS[i].rightLeft == 1)							//0 = car moves left, 1 = car moves right
		{
			mvCarsLeft(game, carMap, FROG, CARS, CONFIG, i, STORK);
		}
	}
}

void initCars(char* carMap, CAR* CARS, Config* CONFIG)
{
	int value;

	for (int i = 0; i < CAR_NUMBER; i++)
	{
		value = (rand() % (CONFIG->game_height - 4)) + 2;
		CARS[i].rightLeft = rand() % 2;
		while ((carMap[value] == '=') || (carMap[value] == 'C')) //checks if planned coordinate is safe zone or other car lane
		{
			value = (rand() % (CONFIG->game_height - 4)) + 2;
		}
		CARS[i].y = value;
		carMap[value] = 'C';
		CARS[i].automobile = CONFIG->car_character;
		CARS[i].speed = (rand() % 5) + 1;
		CARS[i].initializeCounter = rand() % 100;
		//car random speeds
		CARS[i].carSpeed = (rand() % 3) * 3; //speed -> frames that car skip before moving
		CARS[i].carSpeedCounter = 0; // way to count how many frames car have skipped
		if (rand() % NEUTRAL_CHANCE == 0)							//low chance for change of a speed
		{
			CARS[i].neutral = 1;
		}
		if (((rand() % FRIENDLY_CHANCE) == 0) && (CARS[i].neutral != 1))							//low chance for change of a speed
		{
			CARS[i].friendly = 1;
		}
		if (CARS[i].rightLeft == 0) //car will be moving left
		{
			CARS[i].x = 1;
		}
		else if ((CARS[i].rightLeft == 1)) // car will be moving right
		{
			CARS[i].x = CONFIG->game_width - 2;
		}
	}
}

void checkColision(PLAYER* FROG, CAR* CARS)
{
	if (FROG->taxiRequest)   //colisions don't work when car is being transported by friendly car
	{
		return;
	}
	for (int i = 0; i < CAR_NUMBER; i++)
	{
		if (CARS[i].x == FROG->x && CARS[i].y == FROG->y)
		{
			//printw("CRASH!");
			FROG->crash = 1;
			break;
		}
	}
}

void initCurses()
{
	initscr();
	start_color();
	init_pair(1, COLOR_GREEN, COLOR_BLACK);			  //frog colors
	init_pair(2, COLOR_BLUE, COLOR_BLACK);			  //water colors
	init_pair(3, COLOR_RED, COLOR_BLACK);			  //default car color
	init_pair(4, COLOR_YELLOW, COLOR_BLACK);	   	  //optional car color
	init_pair(5, COLOR_MAGENTA, COLOR_BLACK);		  //optional car color
	init_pair(6, COLOR_CYAN, COLOR_BLACK);			  //optional car color
	cbreak();

}

void welcome(PLAYER* FROG)
{

	mvprintw(1, 5, "XXXX  X  X  X   X  XXX   X  X  X   XX");
	mvprintw(2, 5, "   X  X  X  XX XX  X  XX X  XX X  X  X");
	mvprintw(3, 5, "   X  X  X  X X X  X  X  X  XX X  X");
	mvprintw(4, 5, "   X  X  X  X   X  XXX   X  X XX  X XX ");
	mvprintw(5, 5, "   X  X  X  X   X  X     X  X XX  X  X");
	mvprintw(6, 5, "X  X  X  X  X   X  X     X  X  X  X  X");
	mvprintw(7, 5, " XX    XX   X   X  X     X  X  X   XX");
	mvprintw(10, 15, "XXXX  XXX    XX    XX");
	mvprintw(11, 15, "X     X  X  X  X  X  X");
	mvprintw(12, 15, "X     X  X  X  X  X");
	mvprintw(13, 15, "XXXX  XXX   X  X  X XX");
	mvprintw(14, 15, "X     XX    X  X  X  X");
	mvprintw(15, 15, "X     X X   X  X  X  X");
	mvprintw(16, 15, "X     X  X   XX    XX");
	mvprintw(20, 20, " @v@");
	mvprintw(21, 20, "(---)");
	mvprintw(23, 5, "Please enter your username:");
	mvgetnstr(23, 32, FROG->username, 25);
	mvprintw(25, 5, "press any key to continue . . .");
	getch();
	clear();
}

void readRanking(SCORE* RANKS)
{
	FILE* rankingFile = fopen(SCORE_NAME, "r");
	if (rankingFile == NULL) {
		for (int i = 0; i < 3; i++)
		{
			RANKS[i].points = i * 100;
			strcpy(RANKS[i].name, "<computer>");
		}
		return;
	}
	else
	{
		for (int i = 0; i < 3; i++) {
			if (fscanf(rankingFile, "%s\n", &RANKS[i].name) != 1)    //passed as value
			{
				strcpy(RANKS[i].name, "<computer>");
			}
			if (fscanf(rankingFile, "%d\n", &RANKS[i].points) != 1)
			{
				RANKS[i].points = i * 100;
			}
		}
	}
	fclose(rankingFile);
}

//complete rework of this function
void updateRanking(SCORE* RANKS, PLAYER* FROG)
{
	int tempPoints;
	char tempuser[25];

	if (FROG->points >= RANKS[0].points)
	{
		tempPoints = RANKS[0].points;
		strcpy(tempuser, RANKS[0].name);

		RANKS[0].points = FROG->points;
		strcpy(RANKS[0].name, FROG->username);

		RANKS[1].points = RANKS[0].points;
		strcpy(RANKS[1].name, RANKS[0].name);

		RANKS[0].points = tempPoints;
		strcpy(RANKS[0].name, tempuser);
		return;
	}
	else if (FROG->points >= RANKS[1].points)
	{
		RANKS[1].points = FROG->points;
		strcpy(RANKS[1].name, FROG->username);

		RANKS[1].points = RANKS[0].points;
		strcpy(RANKS[1].name, RANKS[0].name);
		return;
	}
	else if (FROG->points >= RANKS[2].points)
	{
		RANKS[2].points = FROG->points;
		strcpy(RANKS[2].name, FROG->username);
		return;
	}
}

void saveRanking(SCORE* RANKS)
{
	FILE* rankingFile = fopen(SCORE_NAME, "w");
	if (rankingFile == NULL)
	{
		return;
	}
	else
	{
		for (int i = 0; i < 3; i++) {
			fprintf(rankingFile, "%s\n", RANKS[i].name);  //passed as value
			fprintf(rankingFile, "%d\n", RANKS[i].points);
		}
	}
	fclose(rankingFile);
}

int handleGameOver(PLAYER* FROG, SCORE* RANKS)
{
	if (FROG->gameOver == 0)
	{
		return 0;
	}
	readRanking(RANKS);
	updateRanking(RANKS, FROG);
	saveRanking(RANKS);
	clear();
	timeout(-1);

	mvprintw(0, 0, "Game over! You scored %d points, bravo!!!", FROG->points); //maybe a window, later as a treat
	mvprintw(1, 5, "=====SCOREBOARD=====");
	mvprintw(2, 1, "1. %s %d", RANKS[2].name, RANKS[2].points);
	mvprintw(3, 1, "2. %s %d", RANKS[1].name, RANKS[1].points);
	mvprintw(4, 1, "3. %s %d", RANKS[0].name, RANKS[0].points);
	mvprintw(6, 3, "press k to exit. . .");
	refresh();

	char k = '-';
	while (k != 'k')
	{
		k = getch();
	}
	return 1;
}

//change clocks to the structs, see if it work
void checkInterval(clock_t interval, clock_t sinterval, PLAYER* FROG, BIRD* STORK)
{
	clock_t end = interval + JUMP_DELAY * CLOCKS_PER_SEC / 1000;
	clock_t storkend = sinterval + JUMP_DELAY * CLOCKS_PER_SEC / 100; //stork having two times longer interval
	if (clock() > end)
	{
		FROG->interval = 0;
	}
	else
	{
		while (getch() != ERR)//cleaning buffer
		{

		}
	}

	if (clock() > storkend)
	{
		STORK->intervalPassed = 0;
	}
}

//=========FILE OPERATIONS================

//rewrite
int get_color_number(const char* color) {
	if (strcmp(color, "RED") == 0) {
		return 3;
	}
	else if (strcmp(color, "YELLOW") == 0) {
		return 4;
	}
	else if (strcmp(color, "MAGENTA") == 0) {
		return 5;
	}
	else if (strcmp(color, "CYAN") == 0) {
		return 6;
	}
	else if (strcmp(color, "GREEN") == 0) {
		return 1;
	}
	else if (strcmp(color, "BLUE") == 0) {
		return 2;
	}
	else {
		return 0;
	}
}

//rewrite
void loadConfig(const char* configFilename, Config* CONFIG, PLAYER* FROG) { //change so it uses default values instead of errors
	FILE* configFile = fopen(configFilename, "r");

	if (configFile == NULL) {
		CONFIG->game_width = 50;
		CONFIG->game_height = 28;
		FROG->frog = 'F';
		CONFIG->car_character = 'C';
		CONFIG->car_color = 3;
		CONFIG->game_speed = 50;
		return;
	}

	if (fscanf(configFile, "game_width=%d\n", &CONFIG->game_width) != 1) {//podobnie jak scanf
		CONFIG->game_width = 50;
	}
	//if (fscanf(configFile, "game_height=%d\n", &config->game_height) != 1) {
	//	
	//}
	CONFIG->game_height = 25;
	if (fscanf(configFile, "player_character=%c\n", &FROG->frog) != 1) {
		FROG->frog = 'F';
	}
	if (fscanf(configFile, "car_character=%c\n", &CONFIG->car_character) != 1) {
		CONFIG->car_character = 'C';
	}
	char color[10];
	if (fscanf(configFile, "car_color=%s\n", color) != 1) {
		CONFIG->car_color = 3;
	}
	else {
		CONFIG->car_color = get_color_number(color);
	}

	if (fscanf(configFile, "game_speed=%d\n", &CONFIG->game_speed) != 1) {
		CONFIG->game_speed = 50;
	}

	fclose(configFile);
}

//=========MAIN FUNCTION==================
//hide clock_t in structs
int main()
{
	BIRD STORK;
	CAR CARS[CAR_NUMBER];
	OBSTACLE ROCK[OBSTACLE_COUNT];
	Config CONFIG;
	PLAYER FROG;
	SCORE RANKS[3];
	srand(time(NULL));
	loadConfig(CONFIG_NAME, &CONFIG, &FROG);
	initPlayer(&FROG, &STORK, &CONFIG); // into the initialize game?
	initCurses();
	welcome(&FROG);
	clock_t start = clock(); //for game countdown
	clock_t interval = clock(); //check if this can be moved somewhere, especially that is used here only once.
	clock_t sinterval = clock();

	WINDOW* game = newwin(CONFIG.game_height, CONFIG.game_width, 4, 3); //test if this can be moved somwhere else CreateWins();
	WINDOW* statbox = newwin(13, 20, 4, 6 + CONFIG.game_width);
	char* carMap = (char*)malloc((CONFIG.game_height - 2) * sizeof(char));
	refresh();
	initGame(game, &FROG, carMap, ROCK, &CONFIG);
	initStats(statbox, &FROG, start);
	initCars(carMap, CARS, &CONFIG);

	// MAIN GAME LOOP
	while (true)
	{
		if (FROG.water == 1 || FROG.crash == 1) //function
		{
			refreshGame(game, &FROG, &CONFIG);
		}

		if (handleGameOver(&FROG, RANKS))
		{
			break;
		}

		checkRequest(game, &FROG, CARS);
		checkInterval(interval, sinterval, &FROG, &STORK);
		handleMove(game, &FROG, carMap, &interval, ROCK, &CONFIG);
		checkColision(&FROG, CARS);

		mvCars(game, carMap, &FROG, CARS, &CONFIG, &STORK);
		mvTaxi(game, &FROG, CARS, &CONFIG);
		//moveStork(&storkInterval)
		checkColision(&FROG, CARS);
		moveStork(game, &STORK, &sinterval, &FROG, carMap, ROCK);
		initStats(statbox, &FROG, start);
		sleep(CONFIG.game_speed);  //CONFIG.game_speed inside

	}
	getch();
	endwin();
}
