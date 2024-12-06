//Mateusz Grodecki 198385
/*		 TO DO LIST:
		*finish snake tutorial
		*colision "roszada bug"
		* vertical maps for left and right borders. (why?)
		* add lives
		* car formula = gameheight = x  -> (x-5)-((x-5)/4)-1
		* curses resizing console window
		* fix leftright
		* make car functions more compact
		* name and surname at the status area
		* car proximiuty move problem, maybe put it inside move function and pass it into request as parameter
		* obstacle leaving taxi problem  check only w and s move scenarios

*/
#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <curses.h>
#include <string.h>
#include <time.h>
using namespace std;


#define CAR_NUMBER 14 //14 by default other for testing
#define TIMER 90 //time for game
#define JUMP_DELAY 150
#define CONFIG_NAME "frogconfig.txt"
#define OBSTACLE_COUNT 30
#define NEUTRAL_CHANCE 31
#define FRIENDLY_CHANCE 2
#define REQUEST_KEY 'e'

//=====DATA STRUCTURES================
typedef struct {
	char frog;
	int y;
	int x; //starting coordinates fix the middle
	int points;
	int moves;
	int water;	 //win condition reached
	int gameOver;		 //time ended in future, end of lives
	int crash;			 //collision
	int taxiRequest;
	int taxi;
	int interval;
	char username[25]; //for scoreboard in future.
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
	// [later] lenght
}CAR;

typedef struct {
	int game_width;
	int game_height;
	char player_character;
	char car_character;
	int car_color;
	int game_speed;
} Config;

typedef struct {
	int y, x;
	char ROCK;
}OBSTACLE;

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

int checkRock(int y, int x, OBSTACLE* ROCK)
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

void initPlayer(PLAYER* FROG)
{
	FROG->points = 0;
	FROG->moves = 0;
	FROG->water = 0;
	FROG->gameOver = 0;
	FROG->crash = 0;
	FROG->taxiRequest = 0;
	FROG->taxi = -1;
	FROG->interval = 0;
}

void checkRequest(WINDOW* gamewin, PLAYER* FROG, CAR* CARS)
{
	//int taxi is needed ffor knowing which car will be our "taxi" - which CAR[taxi] coordinates will our player get
	int whichCar = -1;
	char request = '.'; 
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
			request = getch();
		}
	}
	if (CARS[whichCar].rightLeft == 1)
	{
		if (CARS[whichCar].x == (FROG->x + 1 ))
		{
			request = getch(); //this propably makes car unable to move while near friendlly
		}
	}

	if (request == REQUEST_KEY)
	{
		FROG->taxiRequest = 1;
		FROG->taxi = whichCar;
		mvwaddch(gamewin, FROG->y, FROG->x, ' '); //frog "gets in" to the car -> it disappears from the screen
		printw("request succesfully sent"); //debbuging purpouse only
	}
}

void reprint(WINDOW* gamewin,PLAYER* FROG)
{
	wattron(gamewin, COLOR_PAIR(1));
	mvwaddch(gamewin, FROG->y, FROG->x, FROG->frog);
	wattroff(gamewin, COLOR_PAIR(1));
}

void handleMove(WINDOW* gamewin, PLAYER* FROG, char* verticalMap, clock_t* interval, OBSTACLE* ROCK, Config* CONFIG)
{
	//ROCK COLLISION PRINTING MIGHT NOT BE NEEDED
	//CHANGING TO SWITCH MIGHT BE MORE COMPACT
	if (FROG->interval == 1)
	{
		return;
	}
	char move = getch(); //ERR;
	char buff;

	//while ((buff = getch()) != ERR)
	//{
	//	move = buff;
	//}
	//if (move == ERR)
	//{
	//	return;
	//}

	FROG->taxiRequest = 0; //frog gets out of a taxi, needs another request
	if (verticalMap[FROG->y] == '=')
	{
		mvwaddch(gamewin, FROG->y, FROG->x, '-');
	}
	//else if ((*friendlyRequest) == 1)		CHECK IF THIHS IS NEEDED MANY NEW LINES
	//{
	//	wattron(gamewin, COLOR_PAIR(5));
	//	mvwaddch(gamewin, *yPos, *xPos, config.car_character);
	//	wattroff(gamewin, COLOR_PAIR(5));
	//}
	else
	{
		mvwaddch(gamewin, FROG->y, FROG->x, ' ');
	}
	if (move == 'w')
	{
		//function for checking whats on the board.
		if (checkRock(FROG->y - 1, FROG->x, ROCK))
		{
			reprint(gamewin, FROG);
			return;
		}
		else
		{
			FROG->moves++;
			FROG->y--;
			if (FROG->y == 1)
			{
				FROG->water++;
			}
		}
	}
	else if (move == 'a')
	{
		if (checkRock(FROG->y, FROG->x - 1, ROCK))
		{
			reprint(gamewin, FROG);
			return;
		}
		FROG->moves++;
		FROG->x--;
		if (FROG->x < 1)
		{
			FROG->x = 1;
		}
	}
	else if (move == 's')
	{
		if (checkRock(FROG->y + 1, FROG->x, ROCK))
		{
			reprint(gamewin, FROG);
			return;
		}
		FROG->moves++;
		FROG->y++;
		if (FROG->y > CONFIG->game_height - 2)
		{
			FROG->y = CONFIG->game_height - 2;
		}
	}
	else if (move == 'd')
	{
		if (checkRock(FROG->y, FROG->x + 1, ROCK))
		{
			reprint(gamewin, FROG);
			return;
		}
		FROG->moves++;
		FROG->x++;
		if (FROG->x > CONFIG->game_width - 2)
		{
			FROG->x = CONFIG->game_width - 2;
		}
	}
	if (move != ERR)
	{
		*interval = clock();
		FROG->interval = 1;
	}
	reprint(gamewin, FROG);
	wrefresh(gamewin);



}

void initializeGame(WINDOW* gamewin, PLAYER* FROG, char* verticalMap, OBSTACLE* ROCK, Config* CONFIG)
{
	noecho();
	timeout(0);
	curs_set(0);
	int rockCounter = 0;
	wclear(gamewin);
	wrefresh(gamewin);
	box(gamewin, 0, 0);
	refresh();
	wrefresh(gamewin);
	FROG->y = CONFIG->game_height - 2;
	FROG->x = CONFIG->game_width / 2;
	wattron(gamewin, COLOR_PAIR(1));
	mvwaddch(gamewin, FROG->y, FROG->x, FROG->frog);
	wattroff(gamewin, COLOR_PAIR(1));
	mvprintw(1, 2, "reach the water [@] to score 150 points");
	if (CONFIG->game_width > 15)
	{
		mvwprintw(gamewin, 0, 2, "Jumping_frog");
	}

	else
	{
		mvwprintw(gamewin, 0, 2, "frg");
	}
	wattron(gamewin, COLOR_PAIR(2));
	for (int i = 1; i < CONFIG->game_width - 1; i++)
	{
		mvwprintw(gamewin, 1, i, "@");
	}
	wattroff(gamewin, COLOR_PAIR(2));
	for (int i = 2; i < CONFIG->game_height - 2; i++)
	{
		if (i % 4 == 2)
		{
			for (int j = 1; j < CONFIG->game_width - 1; j++)
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

void refreshGame(WINDOW* gamewin, PLAYER* FROG, Config* CONFIG)
{
	if (FROG->crash == 1)
	{
		FROG->points -= 300;
		FROG->crash--;
		wattron(gamewin, COLOR_PAIR(2));
		mvwprintw(gamewin, FROG->y, FROG->x, " ");
		wattroff(gamewin, COLOR_PAIR(2));
	}
	if (FROG->water == 1)
	{
		FROG->points += 200;
		FROG->water--;
		wattron(gamewin, COLOR_PAIR(2));
		mvwprintw(gamewin, FROG->y, FROG->x, "@");
		wattroff(gamewin, COLOR_PAIR(2));
	}

	FROG->y = CONFIG->game_height - 2;
	FROG->x = CONFIG->game_width / 2;

	wattron(gamewin, COLOR_PAIR(1));
	mvwprintw(gamewin, FROG->y, FROG->x, "F");
	wattroff(gamewin, COLOR_PAIR(1));

	refresh();
	wrefresh(gamewin);
}

void initializeStats(WINDOW* statbox, PLAYER* FROG, clock_t start)
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
	mvwprintw(statbox, 6, 2, "Time left: %d", secondsLeft);
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
	if (CARS[carNum].rightLeft == 0)
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

void moveCarsRight(WINDOW* gamewin, char* verticalMap, PLAYER* FROG, CAR* CARS, Config* CONFIG)
{
	int randomValue = (rand() % (CONFIG->game_height - 4)) + 2;
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

		if (CARS[i].rightLeft == 0)							//0 = car moves left, 1 = car moves right
		{
			CARS[i].carSpeedCounter = 0;

			if ((CARS[i].neutral == 1) || (CARS[i].friendly == 1))//certain cars stopping when frog is in front of them
			{
				if (checkFrogProximity(i, FROG, CARS))  //frog -> pointer to the structure
				{
					break;
				}
			}
			mvwaddch(gamewin, CARS[i].y, CARS[i].x, ' ');
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
					CARS[i].x = 1;
					verticalMap[CARS[i].y] = ' ';//car position from the verticalMap is cleared

					while ((verticalMap[randomValue] == '=') || (verticalMap[randomValue] == 'C')) //checks if planned coordinate is safe zone or other car lane
					{
						randomValue = (rand() % (CONFIG->game_height - 4)) + 2;
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
			if ((CARS[i].neutral == 1))
			{
				wattron(gamewin, COLOR_PAIR(6));
				mvwaddch(gamewin, CARS[i].y, CARS[i].x, CARS[i].automobile);
				wattroff(gamewin, COLOR_PAIR(6));
			}
			else if ((CARS[i].friendly == 1))
			{
				wattron(gamewin, COLOR_PAIR(5));
				mvwaddch(gamewin, CARS[i].y, CARS[i].x, CARS[i].automobile);
				wattroff(gamewin, COLOR_PAIR(5));
			}
			else
			{
				wattron(gamewin, COLOR_PAIR(CONFIG->car_color));
				mvwaddch(gamewin, CARS[i].y, CARS[i].x, CARS[i].automobile);
				wattroff(gamewin, COLOR_PAIR(CONFIG->car_color));
			}
			

		}
	}
	refresh();
	wrefresh(gamewin);
}

void moveCarsLeft(WINDOW* gamewin, char* verticalMap, PLAYER* FROG, CAR* CARS, Config* CONFIG)
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
		randomValue = (rand() % (CONFIG->game_height - 4)) + 2;
		if (CARS[i].rightLeft == 1) //car is in leftMove mode
		{
			CARS[i].carSpeedCounter = 0;
			if ((CARS[i].neutral == 1) || (CARS[i].friendly == 1))
			{
				if (checkFrogProximity(i, FROG, CARS))
				{
					break;
				}
			}
			mvwaddch(gamewin, CARS[i].y, CARS[i].x, ' ');
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
					CARS[i].x = CONFIG->game_width - 2;                // car x is reseted
					verticalMap[CARS[i].y] = ' ';//car position from the map is cleared

					while ((verticalMap[randomValue] == '=') || (verticalMap[randomValue] == 'C')) //checks if planned coordinate is safe zone or other car lane
					{
						randomValue = (rand() % (CONFIG->game_height - 4)) + 2;
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
			
			if ((CARS[i].neutral == 1))
			{
				wattron(gamewin, COLOR_PAIR(6));
				mvwaddch(gamewin, CARS[i].y, CARS[i].x, CARS[i].automobile);
				wattroff(gamewin, COLOR_PAIR(6));
			}
			else if ((CARS[i].friendly == 1))
			{
				wattron(gamewin, COLOR_PAIR(5));
				mvwaddch(gamewin, CARS[i].y, CARS[i].x, CARS[i].automobile);
				wattroff(gamewin, COLOR_PAIR(5));
			}
			else
			{
				wattron(gamewin, COLOR_PAIR(CONFIG->car_color));
				mvwaddch(gamewin, CARS[i].y, CARS[i].x, CARS[i].automobile);
				wattroff(gamewin, COLOR_PAIR(CONFIG->car_color));
			}
		}
	}
	refresh();
	wrefresh(gamewin);
}

void mvTaxi(WINDOW* gamewin, PLAYER* FROG, CAR* CARS, Config* CONFIG)
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
			wattron(gamewin, COLOR_PAIR(1));
			mvwaddch(gamewin, FROG->y, FROG->x, CONFIG->player_character);
			wattroff(gamewin, COLOR_PAIR(1));
		}
	}
	if (CARS[FROG->taxi].rightLeft == 0)
	{
		if (CARS[FROG->taxi].x == (CONFIG->game_width - 2))
		{
			FROG->taxiRequest = 0;
			FROG->x = (CARS[FROG->taxi].x - 1);
			wattron(gamewin, COLOR_PAIR(1));
			mvwaddch(gamewin, FROG->y, FROG->x, CONFIG->player_character);
			wattroff(gamewin, COLOR_PAIR(1));
		}
	}
	refresh();
}

//=====MOST OF THINGS HERE CAN BE COMMON, ONLY FEW THINGGS ARE DISTINCT BASED ON LEFTRIGHT VALUE CHANGE ITTTT================
void initializeCars(char* verticalMap, CAR* CARS, Config* CONFIG)
{
	int value;

	for (int i = 0; i < CAR_NUMBER; i++)
	{
		value = (rand() % (CONFIG->game_height - 4)) + 2;
		CARS[i].rightLeft = rand() % 2;
		while ((verticalMap[value] == '=') || (verticalMap[value] == 'C')) //checks if planned coordinate is safe zone or other car lane
		{
			value = (rand() % (CONFIG->game_height - 4)) + 2;
		}
		CARS[i].y = value;
		verticalMap[value] = 'C';
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
			FROG->crash++;
			break;
		}
	}

}

void initializeCurses()
{
	initscr();
	// LETS CHECK IF SLEEP IS BETTER
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

void handleGameOver(PLAYER* FROG)
{
	clear();
	timeout(-1);
	mvprintw(1, 5, "Game over! You scored %d points, bravo!!!", FROG->points);
	mvprintw(2, 5, "Press k to exit...");
	char k = '-';
	while (k != 'k')
	{
		k = getch();
	}
	//different screens for different points, ranking system
}

void checkInterval(clock_t interval, PLAYER* FROG)
{
	clock_t end = interval + JUMP_DELAY * CLOCKS_PER_SEC / 1000;
	//clock_t storkend = interval + JUMP_DELAY * CLOCKS_PER_SEC / 2000; //stork having two times longer interval
	if (clock() > end)
	{
		FROG->interval = 0;
	}
	//if (clock() > storkend)
	//{
	//	*storkInterval = 0;
	//}
	else
	{
		while (getch() != ERR)//cleaning buffer
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

void loadConfiguration(const char* configFilename, Config* CONFIG, PLAYER* FROG) { //change so it uses default values instead of errors
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
int main()
{
	CAR CARS[CAR_NUMBER];
	OBSTACLE ROCK[OBSTACLE_COUNT];
	Config CONFIG;
	PLAYER FROG;
	loadConfiguration(CONFIG_NAME, &CONFIG, &FROG);
	initPlayer(&FROG);
	srand(time(NULL));
	clock_t start = clock(); //for game countdown
	clock_t interval = clock(); //check if this can be moved somewhere, especially that is used here only once.
	//stork -> to the structt===UNEXISTING STORK DATA
	//int storkInterval = 0;
	//int sY = 2;
	//int sX = 1;
	initializeCurses();
	welcomeScreen();
	WINDOW* gamewin = newwin(CONFIG.game_height, CONFIG.game_width, 4, 3);
	WINDOW* statbox = newwin(10, 20, 4, 6 + CONFIG.game_width);
	char* verticalMap = (char*)malloc((CONFIG.game_height - 2) * sizeof(char));
	refresh();
	initializeGame(gamewin, &FROG, verticalMap, ROCK, &CONFIG);
	initializeStats(statbox, &FROG, start);
	initializeCars(verticalMap, CARS, &CONFIG);

	// MAIN GAME LOOP
	while (true)
	{
		if (FROG.water == 1 || FROG.crash == 1) //function
		{
			refreshGame(gamewin, &FROG, &CONFIG);
			initializeStats(statbox, &FROG, start);  //check in testing if this is needed because it's propably not
		}
		if (FROG.gameOver == 1)
		{
			handleGameOver(&FROG);
			break;
		}
		checkRequest(gamewin, &FROG, CARS);
		checkInterval(interval, &FROG);
		handleMove(gamewin, &FROG, verticalMap, &interval, ROCK, &CONFIG);
		checkColision(&FROG, CARS);
		mvTaxi(gamewin, &FROG, CARS, &CONFIG);
		moveCarsRight(gamewin, verticalMap, &FROG, CARS, &CONFIG);
		moveCarsLeft(gamewin, verticalMap, &FROG, CARS, &CONFIG);
		//moveStork(&storkInterval)
		checkColision(&FROG, CARS);
		initializeStats(statbox, &FROG, start);
		sleep(CONFIG.game_speed);  //config.game_speed inside

	}
	endwin();
}
