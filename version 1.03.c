
/*		 TO DO LIST:
		*finish snake tutorial
		*colision "roszada bug"
		* vertical maps for left and right borders. (why?)
		* add lives
		* car formula = gameheight = x  -> (x-5)-((x-5)/4)-1
		* curses resizing console window
*/
#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <curses.h>
#include <string.h>
#include <time.h>
using namespace std;
#define CAR_NUMBER 14 //14 by default other for testing
#define TIMER 9999 //time for game
#define JUMP_DELAY 150
#define CONFIG_NAME "frogconfig.txt"
#define OBSTACLE_COUNT 15

typedef struct {
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
}CAR;

CAR CARS[CAR_NUMBER];

typedef struct {
	int game_width;
	int game_height;
	char player_character;
	char car_character;
	int car_color;
	int game_speed;
} Config;

Config config;

typedef struct {
	int y, x;
	char ROCK;
}OBSTACLE;

OBSTACLE ROCK[OBSTACLE_COUNT];

//function that makes car "wait"/ modify their speed. Might be working not as intended
/*
void InitRocks()
{
	for (int i = 0; i < OBSTACLE_COUNT; i++)
	{
		ROCK[i].x = (rand() % (config.game_width - 2)) + 1;
	}
}
*/
void sleep(int ms)
{
	clock_t start = clock();  // clock_t = clock ticks
	clock_t end = start + ms * CLOCKS_PER_SEC / 1000;  // start + tenth * (clock per second)/10 [tenth of a second]

	while (clock() < end)
	{
		//wait...
	}
}

int checkRock(int y, int x)
{
	for (int i = 0; i < OBSTACLE_COUNT; i++)
	{
		if (x == ROCK[i].x && y == ROCK[i].y)
		{
			return 1;
		}

	}
	return 0;
}

void handleMove(WINDOW* gamewin, int* yPos, int* xPos, int yMax, int xMax, char frog, int* waterReached, int* points, int* moves, char* verticalMap, int* intervalPassed, clock_t* interval)
{
	if (*intervalPassed == 1)
	{
		return;
	}
	char move = ERR;
	char buff;

	while ((buff = getch()) != ERR)
	{
		move = buff;
	}
	if (move == ERR)
	{
		return;
	}


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
		if (checkRock((*yPos) - 1, *xPos) == 1)
		{
			wattron(gamewin, COLOR_PAIR(1));
			mvwaddch(gamewin, *yPos, *xPos, frog);
			wattroff(gamewin, COLOR_PAIR(1));
			return;
		}
		else
		{
			(*moves)++;
			(*yPos)--;
			if (*yPos == 1)
			{
				(*waterReached)++;
			}
		}
	}
	else if (move == 'a')
	{
		if (checkRock(*yPos, (*xPos)-1) == 1)
		{
			wattron(gamewin, COLOR_PAIR(1));
			mvwaddch(gamewin, *yPos, *xPos, frog);
			wattroff(gamewin, COLOR_PAIR(1));
			return;
		}
		(*moves)++;
		(*xPos)--;
		if (*xPos < 1)
		{
			*xPos = 1;
		}
	}
	else if (move == 's')
	{
		if (checkRock((*yPos)+1, *xPos) == 1)
		{
			wattron(gamewin, COLOR_PAIR(1));
			mvwaddch(gamewin, *yPos, *xPos, frog);
			wattroff(gamewin, COLOR_PAIR(1));
			return;
		}
		(*moves)++;
		(*yPos)++;
		if (*yPos > yMax - 2)
		{
			*yPos = yMax - 2;
		}
	}
	else if (move == 'd')
	{
		if (checkRock(*yPos, (*xPos) + 1) == 1)
		{
			wattron(gamewin, COLOR_PAIR(1));
			mvwaddch(gamewin, *yPos, *xPos, frog);
			wattroff(gamewin, COLOR_PAIR(1));
			return;
		}
		(*moves)++;
		(*xPos)++;
		if (*xPos > xMax - 2)
		{
			*xPos = xMax - 2;
		}
	}
	if (move != ERR)
	{
		*interval = clock();
		*intervalPassed = 1;
	}
	wattron(gamewin, COLOR_PAIR(1));
	mvwaddch(gamewin, *yPos, *xPos, frog);
	wattroff(gamewin, COLOR_PAIR(1));
	wrefresh(gamewin);



}

//redo -> move only frog and not the cars
void initializeGame(WINDOW* gamewin, int* yPos, int* xPos, int yMax, int xMax, char frog, char* verticalMap)
{
	int rockCounter = 0;
	wclear(gamewin);
	wrefresh(gamewin);
	box(gamewin, 0, 0);
	refresh();
	wrefresh(gamewin);
	*yPos = config.game_height - 2;
	*xPos = config.game_width / 2;
	wattron(gamewin, COLOR_PAIR(1));
	mvwaddch(gamewin, *yPos, *xPos, frog);
	wattroff(gamewin, COLOR_PAIR(1));
	mvprintw(1, 2, "reach the water [@] to score 150 points");
	if (config.game_width > 15)
	{
		mvwprintw(gamewin, 0, 2, "Jumping_frog");
	}

	else
	{
		mvwprintw(gamewin, 0, 2, "frg");
	}
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
				if (rand() % 7 == 0 && rockCounter < OBSTACLE_COUNT)
				{
					mvwprintw(gamewin, i, j, "#");
					ROCK[rockCounter].x = j;
					ROCK[rockCounter].y = i;
					rockCounter++;
				}
				else
				{
					mvwprintw(gamewin, i, j, "-");
					verticalMap[i] = '=';
				}

			}
		}
	}
	wrefresh(gamewin);
}

void refreshGame(WINDOW* gamewin, int* yPos, int* xPos, char frog, int*points, int*crash, int*waterReached)
{
	if (*crash == 1)
	{
		(*points) = (*points) - 300;
		(*crash)--;
		wattron(gamewin, COLOR_PAIR(2));
		mvwprintw(gamewin, *yPos, *xPos, " ");
		wattroff(gamewin, COLOR_PAIR(2));
	}
	if (*waterReached == 1)
	{
		(*points) = (*points) +150;
		(*waterReached)--;
		wattron(gamewin, COLOR_PAIR(2));
		mvwprintw(gamewin, *yPos, *xPos, "@");
		wattroff(gamewin, COLOR_PAIR(2));
	}
	
	*yPos = config.game_height - 2;
	*xPos = config.game_width / 2;

	wattron(gamewin, COLOR_PAIR(1));
	mvwprintw(gamewin, *yPos, *xPos, "F");
	wattroff(gamewin, COLOR_PAIR(1));
	
	refresh();
	wrefresh(gamewin);
}

void initializeStats(WINDOW* statbox, int points, int moves, clock_t start, int* gameOver)
{
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
		randomValue = (rand() % (config.game_height - 4)) + 2;
		if (CARS[i].leftRight == 0) //0 = car moves left, 1 = car moves right
		{
			CARS[i].carSpeedCounter = 0;
			mvwaddch(gamewin, CARS[i].y, CARS[i].x, ' ');
			if (CARS[i].x == config.game_width - 2)
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
						randomValue = (rand() % (config.game_height - 4)) + 2;
					}
					verticalMap[randomValue] = 'C';
					CARS[i].y = randomValue; // car again appears onn the Right side, this time with different Y but still in bounds.
					CARS[i].initializeCounter = CARS[i].initializeCounter = rand() % 150;
					break;
				}
			}
			else
			{
				CARS[i].x++;
			}
			wattron(gamewin, COLOR_PAIR(config.car_color));
			mvwaddch(gamewin, CARS[i].y, CARS[i].x, CARS[i].automobile);
			wattroff(gamewin, COLOR_PAIR(config.car_color));
		}
	}
	refresh();
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
		if (CARS[i].carSpeed >= CARS[i].carSpeedCounter)
		{
			continue;
		}
		randomValue = (rand() % (config.game_height - 4)) + 2;
		if (CARS[i].leftRight == 1) //car is in leftMove mode
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
					CARS[i].x = config.game_width - 2;                // car x is reseted
					verticalMap[CARS[i].y] = ' ';//car position from the map is cleared

					while ((verticalMap[randomValue] == '=') || (verticalMap[randomValue] == 'C')) //checks if planned coordinate is safe zone or other car lane
					{
						randomValue = (rand() % (config.game_height - 4)) + 2;
					}
					verticalMap[randomValue] = 'C';
					CARS[i].y = randomValue; // car again appears onn the left side, this time with different Y but still in bounds.
					CARS[i].initializeCounter = CARS[i].initializeCounter = rand() % 150;
					break;

				}
			}
			else //not in border, just move
			{
				CARS[i].x--;
			}
			wattron(gamewin, COLOR_PAIR(config.car_color));
			mvwaddch(gamewin, CARS[i].y, CARS[i].x, CARS[i].automobile);
			wattroff(gamewin, COLOR_PAIR(config.car_color));
		}
	}
	refresh();
	wrefresh(gamewin);
}

void initializeCars(char* verticalMap)
{
	int value;

	for (int i = 0; i < CAR_NUMBER; i++)  //chwilowa zamiana na i bedace rand() % 8
	{
		value = (rand() % (config.game_height - 4)) + 2;
		CARS[i].leftRight = rand() % 2;
		if (CARS[i].leftRight == 0) //car will be moving left
		{

			while ((verticalMap[value] == '=') || (verticalMap[value] == 'C')) //checks if planned coordinate is safe zone or other car lane
			{
				value = (rand() % (config.game_height - 4)) + 2;
			}
			CARS[i].y = value;
			verticalMap[value] = 'C';
			CARS[i].x = 1;
			CARS[i].automobile = config.car_character;
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
				value = (rand() % (config.game_height - 4)) + 2;
			}
			CARS[i].y = value;
			verticalMap[value] = 'C';
			CARS[i].x = config.game_width - 2;
			CARS[i].automobile = config.car_character;
			CARS[i].speed = (rand() % 5) + 1;
			CARS[i].initializeCounter = rand() % 200;
			//car random speeds
			CARS[i].carSpeed = (rand() % 3) * 5; //speed -> frames that car skip before moving
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
	init_pair(1, COLOR_GREEN, COLOR_BLACK);			  //frog colors
	init_pair(2, COLOR_BLUE, COLOR_BLACK);			  //water colors
	init_pair(3, COLOR_RED, COLOR_BLACK);			  //default car color
	init_pair(4, COLOR_YELLOW, COLOR_BLACK);	   	  //optional car color
	init_pair(5, COLOR_MAGENTA, COLOR_BLACK);		  //optional car color
	init_pair(6, COLOR_CYAN, COLOR_BLACK);			  //optional car color
	//low time color
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
	mvprintw(10, 15, "XXXX  XXX    XX    XX     XX    XX   X   X  XXXX");
	mvprintw(11, 15, "X     X  X  X  X  X  X   X  X  X  X  XX XX  X   ");
	mvprintw(12, 15, "X     X  X  X  X  X      X     X  X  X X X  XX     ");
	mvprintw(13, 15, "XXXX  XXX   X  X  X XX   X XX  XXXX  X   X  XXXX");
	mvprintw(14, 15, "X     XX    X  X  X  X   X  X  X  X  X   X  X     ");
	mvprintw(15, 15, "X     X X   X  X  X  X   X  X  X  X  X   X  X     ");
	mvprintw(16, 15, "X     X  X   XX    XX     XX   X  X  X   X  XXXX");
	mvprintw(20, 20, " @v@");
	mvprintw(21, 20, "(---)");
	mvprintw(25, 5, "press any key to continue . . .");
	getch();
	clear();
}

void handleGameOver(int points)
{
	clear();
	timeout(-1);
	mvprintw(1, 5, "Game over! You scored %d points, bravo!!!", points);
	mvprintw(2, 5, "Press k to exit...", points);
	char k = '-';
	while (k != 'k')
	{
		k = getch();
	}
	//different screens for different points, ranking system
}

void checkInterval(clock_t interval, int* intervalPassed)
{
	clock_t end = interval + JUMP_DELAY * CLOCKS_PER_SEC / 1000;
	if (clock() > end)
	{
		*intervalPassed = 0;
	}
	else
	{
		while (getch() != ERR)
		{

		}
	}
}

//=========FILE OPERATIONS================
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

void loadConfiguration(const char* configFilename, Config* config) { //change so it uses default values instead of errors
	FILE* configFile = fopen(configFilename, "r");

	if (configFile == NULL) {
		config->game_width = 50;
		config->game_height = 28;
		config->player_character = 'F';
		config->car_character = 'C';
		config->car_color = 3;
		config->game_speed = 50;
		return;
	}

	if (fscanf(configFile, "game_width=%d\n", &config->game_width) != 1) {//podobnie jak scanf
		config->game_width = 50;
	}
	//if (fscanf(configFile, "game_height=%d\n", &config->game_height) != 1) {
	//	
	//}
	config->game_height = 25;
	if (fscanf(configFile, "player_character=%c\n", &config->player_character) != 1) {
		config->player_character = 'F';
	}
	if (fscanf(configFile, "car_character=%c\n", &config->car_character) != 1) {
		config->car_character = 'C';
	}
	char color[10];
	if (fscanf(configFile, "car_color=%s\n", color) != 1) {
		config->car_color = 3;
	}
	else {
		config->car_color = get_color_number(color);
	}

	if (fscanf(configFile, "game_speed=%d\n", &config->game_speed) != 1) {
		config->game_speed = 50;
	}

	fclose(configFile);
}

//=========MAIN FUNCTION==================
int main()
{

	loadConfiguration(CONFIG_NAME, &config);
	srand(time(NULL));
	// PLAYER VARIABLES   --> STRUCT
	char frog = config.player_character;     //'F';//config.player_character;
	int yPos;
	int xPos; //starting coordinates fix the middle
	int yMax; //bounds
	int xMax;
	int points = 0;
	int moves = 0;
	int waterReached = 0;		 //win condition reached
	int gameOver = 0;			 //time ended in future, end of lives
	int crash = 0;				 //collision

	clock_t start = clock(); //for game countdown
	int intervalPassed = 0;
	clock_t interval = clock();

	// CURSES START
	initializeCurses();
	welcomeScreen();
	timeout(0);//1ms wait time for getch 1- 1ms  0- instant access
	curs_set(0);
	WINDOW* gamewin = newwin(config.game_height, config.game_width, 4, 3);
	WINDOW* statbox = newwin(10, 20, 4, 6 + config.game_width);
	getmaxyx(gamewin, yMax, xMax);
	char* verticalMap = (char*)malloc((config.game_height - 2) * sizeof(char));
	refresh();
	initializeGame(gamewin, &yPos, &xPos, yMax, xMax, frog, verticalMap);
	initializeStats(statbox, points, moves, start, &gameOver);
	initializeCars(verticalMap);

	// MAIN GAME LOOP
	while (true)
	{
		if (waterReached == 1) //function
		{
			refreshGame(gamewin, &yPos, &xPos, frog, &points, &crash, &waterReached);
			initializeStats(statbox, points, moves, start, &gameOver);
		}
		if (crash == 1) // function
		{
			refreshGame(gamewin, &yPos, &xPos, frog, &points, &crash, &waterReached);
			initializeStats(statbox, points, moves, start, &gameOver);
		}
		if (gameOver == 1)
		{
			handleGameOver(points);
			break;
		}
		checkInterval(interval, &intervalPassed);
		handleMove(gamewin, &yPos, &xPos, yMax, xMax, frog, &waterReached, &points, &moves, verticalMap, &intervalPassed, &interval);
		checkColision(yPos, xPos, &crash);
		moveCarsRight(gamewin, yMax, xMax, verticalMap);
		moveCarsLeft(gamewin, yMax, xMax, verticalMap);
		checkColision(yPos, xPos, &crash);
		initializeStats(statbox, points, moves, start, &gameOver);
		sleep(config.game_speed);  //config.game_speed inside

	}
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

/*
* if (
		fscanf(configFile, "game_height=%d\n", &config->game_height) != 1 ||
		fscanf(configFile, "player_character=%c\n", &config->player_character) != 1 ||
		fscanf(configFile, "car_character=%c\n", &config->car_character) != 1 ||
		fscanf(configFile, "car_color=%s\n", &config->car_color) != 1 ||
		fscanf(configFile, "game_speed=%d\n", &config->game_speed) != 1
		) {
		fprintf(stderr, "Error reading configuration file\n");
		fclose(configFile);
		exit(EXIT_FAILURE);
	}
*/
