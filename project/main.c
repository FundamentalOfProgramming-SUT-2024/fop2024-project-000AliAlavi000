#include <ctype.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <locale.h>

#define FILENAME "users.txt"
#define ROWS_PER_PAGE 20
#define WIDTH 20
#define HEIGHT 55
#define FLOORS 4
#define ROOMS_PER_FLOOR 6
#define ROOMS_PER_WIDTH 2
#define ROOMS_PER_HEIGHT 3

#define WALL '|'
#define FLOOR '.'
#define DOOR '+'
#define CORRIDOR '#'
#define PILLAR 'O'
#define WINDOWW '='
#define STAIR '<'
#define PLAYER '1'
#define TRAP '^'
#define GOLD "🪙"
#define GOLD_IN_MAP 'g'
#define BLACK_GOLD_IN_MAP 'i'
#define SIMPLE_FOOD "🍞"
#define SIMPLE_FOOD_IN_MAP 'f'
#define GREAT_FOOD "🦐"
#define GREAT_FOOD_IN_MAP 'u'
#define MAGICAL_FOOD "🧁"
#define MAGICAL_FOOD_IN_MAP 'x'
#define CORRUPT_FOOD "💀"
#define MACE_IN_MAP 'm'
#define MACE "⚒"	// گرز
#define DAGGER "🗡" // خنجر
#define DAGGER_IN_MAP 'd'
#define MAGIC_WAND "🪄" // عصای جادویی
#define MAGIC_WAND_IN_MAP 'w'
#define NORMAL_ARROW "➳" // تیر عادی
#define NORMAL_ARROW_IN_MAP 'a'
#define SWARD "⚔" // شمشیر
#define SWARD_IN_MAP 's'
#define HEALTH "🩺"
#define HEALTH_IN_MAP 'h'
#define SPEED "🚀"
#define SPEED_IN_MAP 'y'
#define DAMAGE "💥"
#define DAMAGE_IN_MAP 'M'
#define FINISH_GAME "🏆"
#define FINISH_GAME_IN_MAP 'z'
#define MONSTER_DEAMON 'D'
#define MONSTER_FIRE_BREATHING 'F'
#define MONSTER_GIANT 'G'
#define MONSTER_SNAKE 'S'
#define MONSTER_UNDEAD 'U'
#define FULLNESS_MAX 50
#define WEAPONS_COUNT 5
#define POTIONS_COUNT 3

// format: username password email scores golds finish_games time_left
char user_name[50];
int level = 1; // 1:easy 2:medium 3:hard;
int color = 0; // 0:red 1:green 2:blue;
int song = 0;  // 0:none 1:moosh 2: ali
char last_pos;
int flooor = 0;
int every_thing_visible = 0;

typedef struct
{
	int flooor, x, y;  // مختصات شروع زیرجدول
	int width, height; // ابعاد زیرجدول
	int visited;	   // 1:visited 0:
	int kind;		   // 0:simple 1:Treasure 2:Enchant 3:Nightmare
} Room;

typedef struct
{
	int flooor, x, y;
} Cell;

typedef struct
{
	Cell cell;
	int visited;
} Stair;

typedef struct
{
	char name;
	int health;
	Cell cell;
	int moves;
	char last_pos;
} Monster;

struct User
{
	char username[50];
	int scores;
	int golds;
	int finish_games;
	int time_left;
};

enum MonsterPower
{
	D = 1,
	F = 2,
	G = 3,
	S = 4,
	U = 6
};

char map[FLOORS][WIDTH][HEIGHT] = {0};
Room rooms[FLOORS * ROOMS_PER_FLOOR];
Monster monsters[FLOORS * ROOMS_PER_FLOOR * 5] = {0};
int stairs[FLOORS][WIDTH][HEIGHT];
int foods[5] = {0, 0, 0, 0, 0};		 // 1:simple 2:great 3:magical 4:corrupt
int foods_time[5] = {0, 0, 0, 0, 0}; // 1:simple 2:great 3:magical 4:corrupt
int fullness = FULLNESS_MAX + 5;
int weapons[5] = {0, 0, 0, 0, 0};						// MACE DAGGER MAGIC_WAND NORMAL_ARROW SWARD
char weapons_icons[5][5] = {"⚒", "🗡", "🪄", "➳", "⚔"}; // MACE DAGGER MAGIC_WAND NORMAL_ARROW SWARD
int current_weapon = 0;
char *weapons_names[WEAPONS_COUNT] = {"Mace", "Dagger", "Magic Wand", "Normal Arrow", "Sword"};
int weapon_range[WEAPONS_COUNT] = {1, 5, 10, 5, 1};	  // برد سلاح‌ها
int weapon_power[WEAPONS_COUNT] = {5, 12, 15, 5, 10}; // قدرت سلاح‌ها
int potions[3] = {0, 0, 0};							  // HEALTH SPEED DAMAGE
char potions_icons[3][5] = {"🩺", "🚀", "💥"};		  // HEALTH SPEED DAMAGE
char *potions_names[POTIONS_COUNT] = {"Health Potion", "Speed Potion", "Damage Potion"};
int power = 0;
int speed = 0;
int hp = 100;
int gold = 0;

void empty_username();
void add_new_user(const char *message);
int validate_username(const char *username);
int validate_password(const char *password);
int validate_email(const char *email);
int contain_space(const char *text);
void generate_password(int length, char *password);
void save_user(const char *username, const char *password, const char *email);
void login(const char *message);
int login_user(const char *username, const char *password);
int guest_login();
void pre_game_menu();
void forgot_password(const char *username);
char *get_password(const char *username);
void game_setting();
void play_mp3();
void kill_mp3();
void scores_table();
void generate_map();
void draw_corridor(int floor, int x1, int y1, int x2, int y2);
void generate_unique_random_numbers(int a, int b, int result[]); // generate a numbers from 0 to b
void connect_rooms(int floor);
void print_map(char message[]);
void move_player(int *px, int *py, int direction, char *message);
void start_playing();
int contains(int num, int nums[], int size);
Cell get_empty_cell(Room room);
int get_room(Cell c);
int show_stair(int x, int y);
int evey_rooms_in_this_floor_visited(int flooor);
void eat_food(char *message);
void show_inventory();
int show_nightmare(int x, int y);
void manage_foods();
void finish_game();
void draw_sad_animation();
void draw_happy_animation();
void center_text(int y, const char *text);
void manage_monsters(Cell c);
void add_monster(Monster m);
void reset_monsters_moves_in_room(Cell cell);
int main()
{
	setlocale(LC_ALL, "");
	play_mp3();
	initscr();
	clear();
	noecho();
	cbreak();
	curs_set(0);

	int startx = 0;
	int starty = 0;

	char *choices[] = {"1. Create New User", "2. Login", "3. Exit"};
	int n_choices = sizeof(choices) / sizeof(choices[0]);

	int highlight = 1;
	int choice = 0;
	int c;

	int menu_width = 30;
	int menu_height = n_choices + 2;

	startx = (COLS - menu_width) / 2;
	starty = (LINES - menu_height) / 2;

	WINDOW *menu_win = newwin(menu_height, menu_width, starty, startx);
	keypad(menu_win, TRUE);

	while (1)
	{
		box(menu_win, 0, 0);
		for (int i = 0; i < n_choices; ++i)
		{
			if (highlight == i + 1)
			{
				wattron(menu_win, A_REVERSE);
				mvwprintw(menu_win, i + 1, 2, "%s", choices[i]);
				wattroff(menu_win, A_REVERSE);
			}
			else
			{
				mvwprintw(menu_win, i + 1, 2, "%s", choices[i]);
			}
		}
		wrefresh(menu_win);
		c = wgetch(menu_win);

		switch (c)
		{
		case KEY_UP:
			if (highlight == 1)
				highlight = n_choices;
			else
				--highlight;
			break;
		case KEY_DOWN:
			if (highlight == n_choices)
				highlight = 1;
			else
				++highlight;
			break;
		case 10:
			choice = highlight;
			break;
		}

		if (choice == 1)
		{
			// Create New User
			add_new_user("");
		}
		else if (choice == 2)
		{
			// Login
			login("");
		}
		else if (choice == 3)
		{
			break;
		}
		choice = 0;
	}

	endwin();
	kill_mp3();
	return 0;
}

void add_new_user(const char *message)
{
	char username[50], password[50], email[100];

	start_color();
	init_pair(1, COLOR_YELLOW, COLOR_BLACK);
	init_pair(2, COLOR_WHITE, COLOR_BLACK);
	init_pair(3, COLOR_RED, COLOR_BLACK);
	init_pair(4, COLOR_GREEN, COLOR_BLACK);
	init_pair(5, COLOR_BLACK, COLOR_WHITE);

	echo();
	curs_set(1);
	keypad(stdscr, FALSE);
	clear();

	if (strlen(message) > 2)
	{
		attron(COLOR_PAIR(3));
		printw("%s\n", message);
		attroff(COLOR_PAIR(3));
	}

	attron(COLOR_PAIR(1));
	printw("\nCreate New User");
	printw("\n**********************************");
	attroff(COLOR_PAIR(1));

	attron(COLOR_PAIR(1));
	printw("\nUsername:");
	attroff(COLOR_PAIR(1));
	attron(COLOR_PAIR(2));
	getstr(username);
	attroff(COLOR_PAIR(2));

	if (!validate_username(username))
	{
		add_new_user("\nUsername is already registered or contains whitespace.");
		return;
	}

	attron(COLOR_PAIR(1));
	printw("\nDo you want to generate a random password (Y/N):");
	attroff(COLOR_PAIR(1));
	attron(COLOR_PAIR(2));
	char create[10];
	getstr(create);
	attroff(COLOR_PAIR(2));

	if (create[0] == 'Y')
	{
		attron(COLOR_PAIR(1));
		printw("Password:");
		attroff(COLOR_PAIR(1));
		attron(COLOR_PAIR(2));
		generate_password(10, password);
		printw("%s\n", password);
		attroff(COLOR_PAIR(2));
	}
	else
	{
		attron(COLOR_PAIR(1));
		printw("Password:");
		attroff(COLOR_PAIR(1));
		attron(COLOR_PAIR(2));
		getstr(password);
		attroff(COLOR_PAIR(2));
	}

	if (!validate_password(password))
	{
		add_new_user("\nPassword should contain at least 7 chars and lower and "
					 "upper and number or contains whitespace.");
		return;
	}

	attron(COLOR_PAIR(1));
	printw("\nEmail: ");
	attroff(COLOR_PAIR(1));
	attron(COLOR_PAIR(2));
	getstr(email);
	attroff(COLOR_PAIR(2));
	if (!validate_email(email))
	{
		add_new_user("\nPlease Enter Valid Email Address.");
		return;
	}

	attron(COLOR_PAIR(4));
	printw("\nThe user was successfully added...");
	attroff(COLOR_PAIR(4));
	getch();
	clear();
	refresh();
	save_user(username, password, email);

	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);
}

int validate_username(const char *username)
{
	if (contain_space(username))
	{
		return 0;
	}

	FILE *file = fopen(FILENAME, "r");
	if (!file)
		return 1;

	char line[200];
	while (fgets(line, sizeof(line), file))
	{
		char stored_username[50];
		sscanf(line, "%49s", stored_username);
		if (strcmp(username, stored_username) == 0)
		{
			fclose(file);
			return 0;
		}
	}

	fclose(file);
	return 1;
}

int validate_password(const char *password)
{
	if (contain_space(password))
	{
		return 0;
	}

	if (strlen(password) < 7)
		return 0;

	int has_upper = 0, has_lower = 0, has_digit = 0;
	for (int i = 0; password[i] != '\0'; i++)
	{
		if (isupper(password[i]))
			has_upper = 1;
		if (islower(password[i]))
			has_lower = 1;
		if (isdigit(password[i]))
			has_digit = 1;
	}

	return has_upper && has_lower && has_digit;
}

int validate_email(const char *email)
{
	if (contain_space(email))
	{
		return 0;
	}

	const char *at_ptr = strchr(email, '@');
	if (!at_ptr)
		return 0;

	const char *dot_ptr = strchr(at_ptr, '.');
	if (!dot_ptr || dot_ptr == at_ptr + 1 || dot_ptr[1] == '\0')
		return 0;

	return 1;
}

int contain_space(const char *text)
{
	if (strchr(text, ' ') == NULL)
	{
		return 0;
	}
	return 1;
}

void generate_password(int length, char *password)
{
	char charset_upper[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char charset_lower[] = "abcdefghijklmnopqrstuvwxyz";
	char charset_digits[] = "0123456789";
	char charset_all[] =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

	srand(time(NULL));

	password[0] = charset_upper[rand() % strlen(charset_upper)];
	password[1] = charset_lower[rand() % strlen(charset_lower)];
	password[2] = charset_digits[rand() % strlen(charset_digits)];

	for (int i = 3; i < length; i++)
	{
		password[i] = charset_all[rand() % strlen(charset_all)];
	}

	for (int i = 0; i < length; i++)
	{
		int j = rand() % length;
		char temp = password[i];
		password[i] = password[j];
		password[j] = temp;
	}

	password[length] = '\0';
}

void save_user(const char *username, const char *password, const char *email)
{
	FILE *file = fopen(FILENAME, "a");
	if (!file)
	{
		add_new_user("\nTry again please...");
		return;
	}

	int scores = 0;
	int golds = 0;
	int finish_games = 0;
	int time_left = 0;

	fprintf(file, "%s %s %s %d %d %d %d\n", username, password, email, scores, golds, finish_games, time_left);
	fclose(file);
}

void login(const char *message)
{
	char username[50], password[50];

	start_color();
	init_pair(1, COLOR_YELLOW, COLOR_BLACK);
	init_pair(2, COLOR_WHITE, COLOR_BLACK);
	init_pair(3, COLOR_RED, COLOR_BLACK);
	init_pair(4, COLOR_GREEN, COLOR_BLACK);
	init_pair(5, COLOR_BLACK, COLOR_WHITE);

	keypad(stdscr, FALSE);
	echo();
	curs_set(1);
	clear();

	if (strlen(message) > 2)
	{
		attron(COLOR_PAIR(3));
		printw("%s", message);
		printw("\n**********************************");
		attroff(COLOR_PAIR(3));
	}

	attron(COLOR_PAIR(1));
	printw("\nLogin");
	printw("\n**********************************");
	attroff(COLOR_PAIR(1));

	attron(COLOR_PAIR(1));
	printw("\nDo you want to continue as guest mode? (Y/N): ");
	attroff(COLOR_PAIR(1));
	attron(COLOR_PAIR(2));
	char guest[10];
	getstr(guest);
	attroff(COLOR_PAIR(2));

	if (guest[0] == 'Y' || guest[0] == 'y')
	{
		pre_game_menu();
		return;
	}

	attron(COLOR_PAIR(1));
	printw("\nUsername: ");
	attroff(COLOR_PAIR(1));
	attron(COLOR_PAIR(2));
	getstr(username);
	attroff(COLOR_PAIR(2));

	attron(COLOR_PAIR(1));
	printw("\nForgot password (Y/N)?");
	attroff(COLOR_PAIR(1));
	attron(COLOR_PAIR(2));
	char forgot[10];
	getstr(forgot);
	attroff(COLOR_PAIR(2));

	if (forgot[0] == 'Y' || forgot[0] == 'y')
	{
		forgot_password(username);
		return;
	}

	attron(COLOR_PAIR(1));
	printw("\nPassword: ");
	attroff(COLOR_PAIR(1));
	attron(COLOR_PAIR(2));
	getstr(password);
	attroff(COLOR_PAIR(2));

	switch (login_user(username, password))
	{
	case 1:
		empty_username();
		strcpy(user_name, username);
		attron(COLOR_PAIR(4));
		printw("\nThe user %s was successfully added...", user_name);
		attroff(COLOR_PAIR(4));
		pre_game_menu();
		break;

	case 0:
		login("Username not found.");
		return;

	case 2:
		login("Incorrect Password");
		return;

	default:
		break;
	}

	getch();
	clear();
	refresh();
	keypad(stdscr, TRUE);
	noecho();
	curs_set(0);
}

int login_user(const char *username, const char *password)
{
	FILE *file = fopen(FILENAME, "r");
	if (!file)
		return 0;

	char line[200];
	while (fgets(line, sizeof(line), file))
	{
		char stored_username[50], stored_password[50];
		sscanf(line, "%49s %49s", stored_username, stored_password);
		if (strcmp(username, stored_username) == 0)
		{
			if (strcmp(password, stored_password) == 0)
			{
				memset(user_name, 0, sizeof(user_name));
				strcpy(user_name, username);
				user_name[strlen(username)] = '\0';
				fclose(file);
				return 1;
			}
			fclose(file);
			return 2;
		}
	}

	fclose(file);
	return 0;
}

void pre_game_menu()
{
	setlocale(LC_ALL, "");
	initscr();
	clear();
	refresh();
	noecho();
	cbreak();
	curs_set(0);

	char *choices[] = {
		"1. Start New Game",
		"2. Continue Previous Game",
		"3. View Scoreboard",
		"4. Settings",
		"5. Exit"};
	int n_choices = sizeof(choices) / sizeof(choices[0]);

	int highlight = 1;
	int choice = 0;
	int c;

	int menu_width = 40;
	int menu_height = n_choices + 4;

	int startx = (COLS - menu_width) / 2;
	int starty = (LINES - menu_height) / 2;

	WINDOW *menu_win = newwin(menu_height, menu_width, starty, startx);
	keypad(menu_win, TRUE);

	while (1)
	{
		box(menu_win, 0, 0);
		mvwprintw(menu_win, 1, (menu_width - strlen("Pre-Game Menu")) / 2, "Pre-Game Menu");
		for (int i = 0; i < n_choices; ++i)
		{
			if (highlight == i + 1)
			{
				wattron(menu_win, A_REVERSE);
				mvwprintw(menu_win, i + 3, 2, "%s", choices[i]);
				wattroff(menu_win, A_REVERSE);
			}
			else
			{
				mvwprintw(menu_win, i + 3, 2, "%s", choices[i]);
			}
		}
		wrefresh(menu_win);
		c = wgetch(menu_win);

		switch (c)
		{
		case KEY_UP:
			if (highlight == 1)
				highlight = n_choices;
			else
				--highlight;
			break;
		case KEY_DOWN:
			if (highlight == n_choices)
				highlight = 1;
			else
				++highlight;
			break;
		case 10:
			choice = highlight;
			break;
		}

		if (choice == 1)
		{
			// Start New Game
			start_playing();
		}
		else if (choice == 2)
		{
		}
		else if (choice == 3)
		{
			// scores
			scores_table();
		}
		else if (choice == 4)
		{
			// setting
			game_setting();
			play_mp3();
		}
		else if (choice == 5)
		{
			break; // Exit
		}

		choice = 0;
	}

	delwin(menu_win);
	endwin();
	clear();
	refresh();
}

int guest_login()
{
	empty_username();
	strcpy(user_name, "Guest mode");
	attron(COLOR_PAIR(4));
	printw("\nLogged in as Guest...\n");
	attroff(COLOR_PAIR(4));

	getch();
	clear();
	refresh();
	noecho();
	curs_set(0);
	return 1;
}

void empty_username()
{
	for (int i = 0; i < 50; i++)
	{
		user_name[i] = 0;
	}
}

void forgot_password(const char *username)
{
	char password[50];

	start_color();
	init_pair(1, COLOR_YELLOW, COLOR_BLACK);
	init_pair(2, COLOR_WHITE, COLOR_BLACK);
	init_pair(3, COLOR_RED, COLOR_BLACK);
	init_pair(4, COLOR_GREEN, COLOR_BLACK);
	init_pair(5, COLOR_BLACK, COLOR_WHITE);
	init_pair(6, COLOR_CYAN, COLOR_BLACK);

	echo();
	curs_set(1);
	clear();

	attron(COLOR_PAIR(1));
	printw("\nForgot Password");
	printw("\n**********************************");
	attroff(COLOR_PAIR(1));

	attron(COLOR_PAIR(1));
	printw("\nRecover Password: ");
	attroff(COLOR_PAIR(1));
	attron(COLOR_PAIR(2));
	printw("%s", username);
	attroff(COLOR_PAIR(2));

	attron(COLOR_PAIR(6));
	printw("\n\n\t%s", get_password(username));
	attroff(COLOR_PAIR(6));

	getch();
	clear();
	refresh();
	noecho();
	curs_set(0);
}

char *get_password(const char *username)
{
	FILE *file = fopen(FILENAME, "r");
	if (!file)
	{
		return "Try again please.";
	}

	char line[200];
	static char password[50];
	while (fgets(line, sizeof(line), file))
	{
		char stored_username[50], stored_password[50];
		sscanf(line, "%49s %49s", stored_username, stored_password);
		if (strcmp(username, stored_username) == 0)
		{
			fclose(file);
			strcpy(password, stored_password);
			return password;
		}
	}

	fclose(file);
	return "Username not found...";
}

void game_setting()
{
	initscr();
	cbreak();
	noecho();
	curs_set(0);
	if (has_colors())
	{
		start_color();
		init_pair(1, COLOR_RED, COLOR_BLACK);
		init_pair(2, COLOR_GREEN, COLOR_BLACK);
		init_pair(3, COLOR_BLUE, COLOR_BLACK);
	}
	char *difficulty_levels[] = {"Easy", "Medium", "Hard"};
	char *colors[] = {"Red", "Green", "Blue"};
	char *songs[] = {"No Song", "Madrese Mooshha", "Ali Kocholoo"};
	int choice = 0;
	while (1)
	{
		clear();
		mvprintw(0, 0, "Game Settings");
		mvprintw(2, 0, "1. Difficulty Level: %s", difficulty_levels[level]);
		mvprintw(3, 0, "2. Character Color: %s", colors[color]);
		mvprintw(4, 0, "3. Background Music: %s", songs[song]);
		mvprintw(6, 0, "4. Save and Exit");
		mvprintw(8, 0, "Enter number of each item in menu to change.");
		if (has_colors())
		{
			attron(COLOR_PAIR(color + 1));
			mvprintw(10, 0, "Preview of Character Color");
			attroff(COLOR_PAIR(color + 1));
		}
		refresh();
		int ch = getch();
		if (ch == '1')
		{
			level = (level + 1) % 3;
		}
		else if (ch == '2')
		{
			color = (color + 1) % 3;
		}
		else if (ch == '3')
		{
			song = (song + 1) % 3;
		}
		else if (ch == '4')
		{
			break;
		}
	}
	clear();
	refresh();
	endwin();
}

void play_mp3()
{
	char command[256];

	kill_mp3();
	if (song == 0)
	{
		return;
	}
	if (song == 1)
	{
		snprintf(command, sizeof(command), "mpg123 1.mp3 > /dev/null 2>&1 &");
	}
	else if (song == 2)
	{
		snprintf(command, sizeof(command), "mpg123 2.mp3 > /dev/null 2>&1 &");
	}

	system(command);
}

void kill_mp3()
{
	char command[256];
	snprintf(command, sizeof(command), "pkill mpg123");
	system(command);
}

void scores_table()
{
	initscr();
	clear();
	noecho();
	cbreak();
	curs_set(0);
	keypad(stdscr, TRUE);

	// Initialize color pairs
	start_color();
	init_pair(1, COLOR_YELLOW, COLOR_BLACK);
	init_pair(2, COLOR_WHITE, COLOR_BLACK);
	init_pair(3, COLOR_RED, COLOR_BLACK);
	init_pair(4, COLOR_GREEN, COLOR_BLACK);
	init_pair(5, COLOR_BLACK, COLOR_WHITE);
	init_pair(6, COLOR_CYAN, COLOR_BLACK);

	// Open file
	FILE *file = fopen(FILENAME, "r");
	if (!file)
	{
		mvprintw(LINES / 2, (COLS - 30) / 2, "Error: Could not open file.");
		getch();
		endwin();
		return;
	}

	// Load user data
	struct User users[100];
	int count = 0;
	char line[200];
	while (fgets(line, sizeof(line), file) && count < 100)
	{
		sscanf(line, "%49s %*s %*s %d %d %d %d",
			   users[count].username,
			   &users[count].scores,
			   &users[count].golds,
			   &users[count].finish_games,
			   &users[count].time_left);
		count++;
	}
	fclose(file);

	// Sort users by scores
	for (int i = 0; i < count - 1; i++)
	{
		for (int j = i + 1; j < count; j++)
		{
			if (users[j].scores > users[i].scores)
			{
				struct User temp = users[i];
				users[i] = users[j];
				users[j] = temp;
			}
		}
	}

	int current_page = 0;
	int total_pages = (count + ROWS_PER_PAGE - 1) / ROWS_PER_PAGE;
	int scroll_offset = 0;

	strcat(users[0].username, " (king)");
	strcat(users[1].username, " (master)");
	strcat(users[2].username, " (goat)");

	while (1)
	{
		clear();
		int start_row = 2;
		int start_col = (COLS - 60) / 2;

		mvprintw(start_row - 2, start_col, "Top Players - Page %d/%d", current_page + 1, total_pages);
		mvprintw(start_row, start_col, "Rank  Username          Scores  Golds  Games  Time Left");
		mvprintw(start_row + 1, start_col, "-------------------------------------------------------");

		// Define the range of users visible in the current page
		int start_idx = current_page * ROWS_PER_PAGE;
		int end_idx = (start_idx + ROWS_PER_PAGE < count) ? start_idx + ROWS_PER_PAGE : count;

		// Scrollable range within the current page
		int visible_lines = LINES - (start_row + 5); // Remaining lines for display
		int visible_start_idx = start_idx + scroll_offset;
		int visible_end_idx = (visible_start_idx + visible_lines < end_idx) ? visible_start_idx + visible_lines : end_idx;

		for (int i = visible_start_idx; i < visible_end_idx; i++)
		{
			char rank_display[12];
			if (i == 0)
			{
				strcpy(rank_display, "🥇   ");
			}
			else if (i == 1)
			{
				strcpy(rank_display, "🥈   ");
			}
			else if (i == 2)
			{
				strcpy(rank_display, "🥉   ");
			}
			else
				sprintf(rank_display, "%d", i + 1);

			if (i < 3)
				attron(A_BOLD);
			if (i == 0)
				attron(COLOR_PAIR(4));
			else if (i == 1)
				attron(COLOR_PAIR(1));
			else if (i == 2)
				attron(COLOR_PAIR(6));

			mvprintw(start_row + 2 + (i - visible_start_idx), start_col,
					 "%-5s %-16.16s %-7d %-6d %-6d %-9d",
					 rank_display,
					 users[i].username,
					 users[i].scores,
					 users[i].golds,
					 users[i].finish_games,
					 users[i].time_left);

			if (i < 3)
				attroff(A_BOLD);
			if (i == 0)
				attroff(COLOR_PAIR(4));
			else if (i == 1)
				attroff(COLOR_PAIR(1));
			else if (i == 2)
				attroff(COLOR_PAIR(6));
		}

		mvprintw(LINES - 2, (COLS - 40) / 2, "Up/Down: Scroll, Left/Right: Pages, Q: Quit");
		refresh();

		int ch = getch();
		if (ch == 'q' || ch == 'Q')
		{
			break;
		}
		else if (ch == KEY_RIGHT)
		{
			if (current_page < total_pages - 1)
			{
				current_page++;
				scroll_offset = 0; // Reset scroll for new page
			}
		}
		else if (ch == KEY_LEFT)
		{
			if (current_page > 0)
			{
				current_page--;
				scroll_offset = 0; // Reset scroll for new page
			}
		}
		else if (ch == KEY_DOWN)
		{
			if ((scroll_offset + visible_lines) < (end_idx - start_idx))
			{
				scroll_offset++;
			}
		}
		else if (ch == KEY_UP)
		{
			if (scroll_offset > 0)
			{
				scroll_offset--;
			}
		}
	}

	clear();
	refresh();

	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);
	endwin();
}

void generate_map()
{
	hp = 100;
	gold = 0;
	speed = 0;
	power = 0;

	for (int i = 0; i < FLOORS; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			for (int k = 0; k < HEIGHT; k++)
			{
				map[i][j][k] = ' ';
				stairs[i][j][k] = 0;
			}
		}
	}

	for (int i = 0; i < 5; i++)
	{
		foods[i] = 0;
		weapons[i] = 0;
		if (i < 3)
		{
			potions[i] = 0;
		}
	}
	current_weapon = 0;

	Room r;
	r.flooor = 0;
	r.height = 0;
	r.visited = 0;
	r.width = 0;
	r.x = 0;
	r.y = 0;
	for (int i = 0; i < FLOORS * ROOMS_PER_FLOOR; i++)
	{
		rooms[i] = r;
	}

	srand(time(NULL)); // مقداردهی تصادفی

	for (int floor = 0; floor < FLOORS; floor++)
	{
		for (int i = 0; i < ROOMS_PER_WIDTH; i++)
		{
			for (int j = 0; j < ROOMS_PER_HEIGHT; j++)
			{
				Room new_room;
				new_room.flooor = floor;
				new_room.visited = 0;
				new_room.kind = 0;
				if (i == 0 && j == 0 && floor)
				{
					new_room.x = rooms[0].x;
					new_room.y = rooms[0].y;
					new_room.width = rooms[0].width;
					new_room.height = rooms[0].height;
				}
				else
				{
					do
					{
						new_room.height = rand() % 4 + 7;
						new_room.width = rand() % 2 + 6;
						int w = (WIDTH / ROOMS_PER_WIDTH), h = (HEIGHT / ROOMS_PER_HEIGHT);
						new_room.x = (rand() % (w - new_room.width)) + (w * (i % ROOMS_PER_WIDTH) + 2);
						new_room.y = (rand() % (h - new_room.height)) + (h * (j % ROOMS_PER_HEIGHT) + 2);
					} while (new_room.x + new_room.width >= WIDTH - 1 || new_room.y + new_room.height >= HEIGHT - 1);
				}

				int kind = rand() % 10;

				if (kind == 0 && floor == FLOORS - 1)
					new_room.kind = 1;
				else if (kind < 4)
					new_room.kind = 2;
				else if (kind < 6)
					new_room.kind = 3;

				if (floor == FLOORS - 1 && i == ROOMS_PER_WIDTH - 1 && j == ROOMS_PER_HEIGHT - 1)
				{
					new_room.kind = 1;
				}
				else if (!floor && !i && !j)
				{
					new_room.kind = 0;
				}

				rooms[floor * ROOMS_PER_FLOOR + i + (j * ROOMS_PER_WIDTH)] = new_room;

				for (int k = new_room.x; k < new_room.x + new_room.width; k++)
				{
					for (int t = new_room.y; t < new_room.y + new_room.height; t++)
					{
						if (t == new_room.y || t == new_room.y + new_room.height - 1)
						{
							map[floor][k][t] = '|';
						}
						else if (k == new_room.x || k == new_room.x + new_room.width - 1)
						{
							map[floor][k][t] = '_';
						}
						else
						{
							map[floor][k][t] = '.';
						}
						map[floor][new_room.x][new_room.y] = '_';
						map[floor][new_room.x][new_room.y + new_room.height - 1] = '_';
					}
				}

				if (new_room.kind == 1)
				{
					Cell t = get_empty_cell(new_room);
					map[floor][t.y][t.x] = FINISH_GAME_IN_MAP;
				}
			}
		}
		connect_rooms(floor);
	}

	Cell c = get_empty_cell(rooms[0]);
	map[0][c.y][c.x] = PLAYER;

	int sward_room = rand() % (ROOMS_PER_FLOOR);
	Cell stair = get_empty_cell(rooms[0]);
	Cell sward = get_empty_cell(rooms[sward_room]);

	for (int i = 0; i < FLOORS; i++)
	{
		map[i][stair.y][stair.x] = STAIR;
	}
	map[sward.flooor][sward.y][sward.x] = SWARD_IN_MAP;
	weapons[0] = 1;

	for (int floor = 0; floor < FLOORS; floor++)
	{
		for (int i = 0; i < ROOMS_PER_WIDTH; i++)
		{
			for (int j = 0; j < ROOMS_PER_HEIGHT; j++)
			{
				Room new_room = rooms[floor * ROOMS_PER_FLOOR + i + (j * ROOMS_PER_WIDTH)];

				int pillars = 1,
					traps = 1,
					gold = 1,
					black_gold = 1,
					food = 1,
					dagger = 1,
					magic_wand = 1,
					normal_arrow = 1,
					health = 1,
					speed = 1,
					damage = 1,
					deamon = 1,
					fire_breathing = 1,
					giant = 1,
					snake = 1,
					undead = 1;
				if (new_room.kind == 1) // گنج
				{
					pillars = rand() % 4;
					traps = rand() % (6 - level);
					food = rand() % (1 + level);
					dagger = rand() % (8 + level);
					magic_wand = rand() % (8 + level);
					normal_arrow = rand() % (8 + level);
					health = rand() % (8 + level);
					speed = rand() % (8 + level);
					damage = rand() % (8 + level);
					deamon = rand() % (4 - level);
					fire_breathing = rand() % (5 - level);
					giant = rand() % (6 - level);
					undead = rand() % (7 - level);
				}
				else if (new_room.kind == 2) // طلسم
				{
					pillars = rand() % 4;
					gold = rand() % (5 + level);
					black_gold = rand() % (20 + (2 * level));
					food = rand() % (1 + level);
					dagger = rand() % (5 + level);
					magic_wand = rand() % (2 + level);
					normal_arrow = rand() % (1 + level);
					health = rand() % (1 + level);
					speed = rand() % (1 + level);
					damage = rand() % (1 + level);
				}
				else if (new_room.kind == 3) // کابوس
				{
					pillars = rand() % 4;
					gold = rand() % (2 + level);
					black_gold = rand() % (12 + (2 * level));
					food = rand() % (1 + level);
					dagger = rand() % (3 + level);
					magic_wand = rand() % (4 + level);
					normal_arrow = rand() % (3 + level);
					health = rand() % (4 + level);
					speed = rand() % (4 + level);
					damage = rand() % (4 + level);
				}
				else
				{
					pillars = rand() % 4;
					traps = rand() % (6 - level);
					gold = rand() % (1 + level);
					black_gold = rand() % (6 + (2 * level));
					food = rand() % (level);
					dagger = rand() % (3 + level);
					magic_wand = rand() % (4 + level);
					normal_arrow = rand() % (3 + level);
					health = rand() % (4 + level);
					speed = rand() % (4 + level);
					damage = rand() % (4 + level);
					deamon = rand() % (5 - level);
					fire_breathing = rand() % (6 - level);
					giant = rand() % (7 - level);
					undead = rand() % (8 - level);
					snake = rand() % (7 - level);
				}

				if (new_room.kind == 1)
				{
					for (int i = 0; i < 7; i++)
					{
						Cell t = get_empty_cell(new_room);
						map[floor][t.y][t.x] = TRAP;
					}
				}

				Monster m;
				m.moves = 0;
				snake = 0;
				if (!undead)
				{
					Cell t = get_empty_cell(new_room);
					map[floor][t.y][t.x] = MONSTER_UNDEAD;
					t.flooor = floor;
					int tmp = t.x;
					t.x = t.y;
					t.y = tmp;
					m.cell = t;
					m.health = 30;
					m.name = MONSTER_UNDEAD;
					add_monster(m);
				}
				if (!deamon)
				{
					Cell t = get_empty_cell(new_room);
					map[floor][t.y][t.x] = MONSTER_DEAMON;
					t.flooor = floor;
					int tmp = t.x;
					t.x = t.y;
					t.y = tmp;
					m.cell = t;
					m.health = 5;
					m.name = MONSTER_DEAMON;
					add_monster(m);
				}
				if (!fire_breathing)
				{
					Cell t = get_empty_cell(new_room);
					map[floor][t.y][t.x] = MONSTER_FIRE_BREATHING;
					t.flooor = floor;
					int tmp = t.x;
					t.x = t.y;
					t.y = tmp;
					m.cell = t;
					m.health = 10;
					m.name = MONSTER_FIRE_BREATHING;
					add_monster(m);
				}
				if (!giant)
				{
					Cell t = get_empty_cell(new_room);
					map[floor][t.y][t.x] = MONSTER_GIANT;
					t.flooor = floor;
					int tmp = t.x;
					t.x = t.y;
					t.y = tmp;
					m.cell = t;
					m.health = 15;
					m.name = MONSTER_GIANT;
					add_monster(m);
				}
				if (!snake)
				{
					Cell t = get_empty_cell(new_room);
					map[floor][t.y][t.x] = MONSTER_SNAKE;
					t.flooor = floor;
					int tmp = t.x;
					t.x = t.y;
					t.y = tmp;
					m.cell = t;
					m.health = 20;
					m.name = MONSTER_SNAKE;
					m.last_pos = FLOOR;
					add_monster(m);
				}
				if (!traps)
				{
					Cell t = get_empty_cell(new_room);
					map[floor][t.y][t.x] = TRAP;
				}
				if (!health)
				{
					Cell t = get_empty_cell(new_room);
					map[floor][t.y][t.x] = HEALTH_IN_MAP;
				}
				if (!speed)
				{
					Cell t = get_empty_cell(new_room);
					map[floor][t.y][t.x] = SPEED_IN_MAP;
				}
				if (!damage)
				{
					Cell t = get_empty_cell(new_room);
					map[floor][t.y][t.x] = DAMAGE_IN_MAP;
				}
				if (!magic_wand)
				{
					Cell t = get_empty_cell(new_room);
					map[floor][t.y][t.x] = MAGIC_WAND_IN_MAP;
				}
				if (!normal_arrow)
				{
					Cell t = get_empty_cell(new_room);
					map[floor][t.y][t.x] = NORMAL_ARROW_IN_MAP;
				}
				if (!food)
				{
					Cell t = get_empty_cell(new_room);
					int r = rand() % 4;
					if (r == 0)
					{
						map[floor][t.y][t.x] = MAGICAL_FOOD_IN_MAP;
					}
					else if (r == 1)
					{
						map[floor][t.y][t.x] = GREAT_FOOD_IN_MAP;
					}
					else
					{
						map[floor][t.y][t.x] = SIMPLE_FOOD_IN_MAP;
					}
				}
				if (!black_gold)
				{
					Cell t = get_empty_cell(new_room);
					map[floor][t.y][t.x] = BLACK_GOLD_IN_MAP;
				}
				if (!gold)
				{
					Cell t = get_empty_cell(new_room);
					map[floor][t.y][t.x] = GOLD_IN_MAP;
				}
				if (pillars < 3)
				{
					for (int i = 0; i < pillars; i++)
					{
						Cell p = get_empty_cell(new_room);
						map[floor][p.y][p.x] = PILLAR;
					}
				}
			}
		}
	}
}

int contains(int num, int nums[], int size)
{
	for (int i = 0; i < size; i++)
	{
		if (nums[i] == num)
		{
			return 1; // عدد پیدا شد
		}
	}
	return 0; // عدد پیدا نشد
}

void draw_corridor(int floor, int x1, int y1, int x2, int y2)
{
	int dx = abs(x1 - x2); // تفاوت در x
	int dy = abs(y1 - y2); // تفاوت در y

	srand(time(NULL)); // مقداردهی اولیه برای اعداد تصادفی

	map[floor][x1][y1] = '#'; // شروع مسیر
	map[floor][x2][y2] = '#'; // پایان مسیر

	// انتخاب تصادفی جهت‌های افقی و عمودی
	int total_steps = dx + dy;
	int horizontal_steps = dx;
	int vertical_steps = dy;

	// شبیه‌سازی حرکت ترکیبی
	while (horizontal_steps > 0 || vertical_steps > 0)
	{
		int r = rand() % (dx + dy + 1);
		if (r < dx && horizontal_steps > 0) // انتخاب حرکت افقی
		{
			if (x1 < x2)
				x1++;
			else if (x1 > x2)
				x1--;

			map[floor][x1][y1] = '#'; // رسم افقی
			horizontal_steps--;
		}
		else if (r == dx && horizontal_steps > 0 && vertical_steps > 0) // انتخاب حرکت افقی
		{
			if (x1 < x2)
				x1++;
			else if (x1 > x2)
				x1--;

			if (y1 < y2)
				y1++;
			else if (y1 > y2)
				y1--;

			map[floor][x1][y1] = '#'; // رسم افقی
			horizontal_steps--;
			vertical_steps--;
		}
		else if (vertical_steps > 0) // انتخاب حرکت عمودی
		{
			if (y1 < y2)
				y1++;
			else if (y1 > y2)
				y1--;

			map[floor][x1][y1] = '#'; // رسم عمودی
			vertical_steps--;
		}
	}
}

void generate_unique_random_numbers(int a, int b, int result[])
{
	int count = 0;
	int used[b + 1];

	// مقداردهی اولیه آرایه used به 0
	for (int i = 0; i <= b; i++)
	{
		used[i] = 0;
	}

	srand(time(NULL)); // مقداردهی اولیه تصادفی

	while (count < a)
	{
		int random_number = rand() % (b + 1);

		if (!used[random_number])
		{
			result[count] = random_number;
			used[random_number] = 1;
			count++;
		}
	}
}

void connect_rooms(int floor)
{
	for (int i = 0; i < ROOMS_PER_HEIGHT - 1; i++)
	{
		for (int j = 0; j < ROOMS_PER_WIDTH; j++)
		{
			Room r1 = rooms[floor * ROOMS_PER_FLOOR + i * ROOMS_PER_WIDTH + j];
			Room r2 = rooms[floor * ROOMS_PER_FLOOR + (i + 1) * ROOMS_PER_WIDTH + j];

			// انتخاب مختصات در در دیوار پایین اتاق اول
			int door1_x = rand() % (r1.width - 2) + r1.x + 1;
			int door1_y = r1.y + r1.height - 1;

			// انتخاب مختصات در در دیوار بالای اتاق دوم
			int door2_x = rand() % (r2.width - 2) + r2.x + 1;
			int door2_y = r2.y;

			// قرار دادن درها
			map[floor][door1_x][door1_y] = '+';
			map[floor][door2_x][door2_y] = '+';

			draw_corridor(floor, door1_x, door1_y + 1, door2_x, door2_y - 1);
		}
	}

	for (int i = 0; i < ROOMS_PER_WIDTH - 1; i++)
	{
		for (int j = 0; j < ROOMS_PER_HEIGHT; j++)
		{
			Room r1 = rooms[floor * ROOMS_PER_FLOOR + i + j * ROOMS_PER_WIDTH];
			Room r2 = rooms[floor * ROOMS_PER_FLOOR + (i + 1) + j * ROOMS_PER_WIDTH];

			// انتخاب مختصات در در دیوار پایین اتاق اول
			int door1_y = rand() % (r1.height - 2) + r1.y + 1;
			int door1_x = r1.x + r1.width - 1;

			// انتخاب مختصات در در دیوار بالای اتاق دوم
			int door2_y = rand() % (r2.height - 2) + r2.y + 1;
			int door2_x = r2.x;

			// قرار دادن درها
			map[floor][door1_x][door1_y] = '+';
			map[floor][door2_x][door2_y] = '+';

			draw_corridor(floor, door1_x + 1, door1_y, door2_x - 1, door2_y);
		}
	}
}

void print_map(char message[])
{
	start_color();
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_BLUE, COLOR_BLACK);
	init_pair(4, COLOR_YELLOW, COLOR_BLACK);
	init_pair(5, COLOR_WHITE, COLOR_BLACK);
	init_pair(6, COLOR_BLACK, COLOR_YELLOW);
	init_pair(7, COLOR_MAGENTA, COLOR_BLACK);

	printw("%s", message);

	for (int i = 0; i < 10; i++)
	{
		if (LINES % 2 == 0)
		{
			move(LINES / 2 - 5 + i, 0);
		}
		else
		{
			move((LINES + 1) / 2 - 5 + i, 0);
		}

		int per = fullness / 5;
		if (per > i)
		{
			printw("🟩");
		}
		else
		{
			printw("🟥");
		}
	}

	for (int i = 0; i < 5; i++)
	{
		if (LINES % 2 == 0)
		{
			move(LINES / 2 - 3 + i, 3);
		}
		else
		{
			move((LINES + 1) / 2 - 3 + i, 3);
		}
		switch (foods[i])
		{
		case 0:
			printw("__");
			break;

		case 1:
			printw("%s", SIMPLE_FOOD);
			break;
		case 2:
			printw("%s", GREAT_FOOD);
			break;
		case 3:
			printw("%s", MAGICAL_FOOD);
			break;
		case 4:
			printw("%s", SIMPLE_FOOD);
			break;

		default:
			break;
		}
	}

	for (int i = 0; i < 5; i++)
	{
		if (LINES % 2 == 0)
		{
			move(LINES / 2 - 5 + i, COLS - 6);
		}
		else
		{
			move((LINES + 1) / 2 - 5 + i, COLS - 6);
		}
		if (current_weapon == i)
		{
			attron(COLOR_PAIR(4));
		}
		printw("%-5s|%d", weapons_icons[i], weapons[i]);
		if (current_weapon == i)
		{
			attroff(COLOR_PAIR(4));
		}
	}
	for (int i = 0; i < 3; i++)
	{
		if (LINES % 2 == 0)
		{
			move(LINES / 2 + 1 + i, COLS - 6);
		}
		else
		{
			move((LINES + 1) / 2 + 1 + i, COLS - 6);
		}
		printw("%-5s|%d", potions_icons[i], potions[i]);
	}

	int start_x = (LINES - WIDTH) / 2;
	int start_y = (COLS - HEIGHT) / 2;
	Cell c;
	c.flooor = flooor;
	for (int i = 0; i < WIDTH; i++)
	{
		for (int j = 0; j < HEIGHT; j++)
		{
			c.x = i;
			c.y = j;
			int room_index = get_room(c);
			if (stairs[flooor][i][j] || rooms[get_room(c)].visited || every_thing_visible || show_stair(i, j))
			{
				if (room_index != -1 && rooms[room_index].kind == 3 && !show_nightmare(i, j) && !every_thing_visible)
				{
					continue;
				}

				int kind = rooms[get_room(c)].kind;
				if (kind == 1)
					attron(COLOR_PAIR(4));
				else if (kind == 2)
					attron(COLOR_PAIR(3));
				else if (kind == 3)
					attron(COLOR_PAIR(7));

				if (map[flooor][i][j] == PLAYER)
				{
					attron(COLOR_PAIR(color + 1));
					mvprintw(i + start_x, j + start_y, "%c", PLAYER);
					attroff(COLOR_PAIR(color + 1));
				}
				else if (map[flooor][i][j] == TRAP && !stairs[flooor][i][j])
				{
					// attron(COLOR_PAIR(3));
					mvprintw(i + start_x, j + start_y, "%c", FLOOR);
					// attroff(COLOR_PAIR(3));
				}
				else if (map[flooor][i][j] == SIMPLE_FOOD_IN_MAP && !stairs[flooor][i][j])
				{
					mvprintw(i + start_x, j + start_y, "%s", SIMPLE_FOOD);
					j++;
				}
				else if (map[flooor][i][j] == GREAT_FOOD_IN_MAP && !stairs[flooor][i][j])
				{
					mvprintw(i + start_x, j + start_y, "%s", GREAT_FOOD);
					j++;
				}
				else if (map[flooor][i][j] == MAGICAL_FOOD_IN_MAP && !stairs[flooor][i][j])
				{
					mvprintw(i + start_x, j + start_y, "%s", MAGICAL_FOOD);
					j++;
				}
				else if (map[flooor][i][j] == GOLD_IN_MAP && !stairs[flooor][i][j])
				{
					attron(COLOR_PAIR(4));
					mvprintw(i + start_x, j + start_y, "%s", GOLD);
					attroff(COLOR_PAIR(4));
					j++;
				}
				else if (map[flooor][i][j] == BLACK_GOLD_IN_MAP && !stairs[flooor][i][j])
				{
					attron(COLOR_PAIR(6));
					mvprintw(i + start_x, j + start_y, "%s", GOLD);
					attroff(COLOR_PAIR(6));
					j++;
				}
				else if (map[flooor][i][j] == DAGGER_IN_MAP && !stairs[flooor][i][j])
				{
					mvprintw(i + start_x, j + start_y, "%s", DAGGER);
					j++;
				}
				else if (map[flooor][i][j] == MAGIC_WAND_IN_MAP && !stairs[flooor][i][j])
				{
					mvprintw(i + start_x, j + start_y, "%s", MAGIC_WAND);
					j++;
				}
				else if (map[flooor][i][j] == NORMAL_ARROW_IN_MAP && !stairs[flooor][i][j])
				{
					mvprintw(i + start_x, j + start_y, "%s", NORMAL_ARROW);
					j++;
				}
				else if (map[flooor][i][j] == SWARD_IN_MAP && !stairs[flooor][i][j])
				{
					mvprintw(i + start_x, j + start_y, "%s", SWARD);
					j++;
				}
				else if (map[flooor][i][j] == HEALTH_IN_MAP && !stairs[flooor][i][j])
				{
					mvprintw(i + start_x, j + start_y, "%s", HEALTH);
					j++;
				}
				else if (map[flooor][i][j] == SPEED_IN_MAP && !stairs[flooor][i][j])
				{
					mvprintw(i + start_x, j + start_y, "%s", SPEED);
					j++;
				}
				else if (map[flooor][i][j] == DAMAGE_IN_MAP && !stairs[flooor][i][j])
				{
					mvprintw(i + start_x, j + start_y, "%s", DAMAGE);
					j++;
				}
				else if (map[flooor][i][j] == FINISH_GAME_IN_MAP && !stairs[flooor][i][j])
				{
					mvprintw(i + start_x, j + start_y, "%s", FINISH_GAME);
					j++;
				}
				else
				{
					mvprintw(i + start_x, j + start_y, "%c", map[flooor][i][j]);
				}

				if (kind == 1)
					attroff(COLOR_PAIR(4));
				else if (kind == 2)
					attroff(COLOR_PAIR(3));
				else if (kind == 3)
					attroff(COLOR_PAIR(7));
			}
		}
	}

	move(LINES - 1, 0);
	printw("Floor: %d\tHP: %d\tGold: %d\tSpeed: %d\tPower: %d", flooor + 1, hp, gold, speed, power);
	refresh();
}

void move_player(int *px, int *py, int direction, char *message)
{
	for (int i = 0; i < 150; i++)
	{
		message[i] = ' ';
	}
	int new_x = *px;
	int new_y = *py;
	int press_f = FALSE;
	int press_g = FALSE;
	int counter = 0;

	if (direction == 'f' || direction == 'F')
	{
		press_f = TRUE;
		while ((direction = getch()))
		{
			if (direction == 'f')
			{
				press_f = FALSE;
				break;
			}
			if (direction >= '1' && direction <= '9')
			{
				break;
			}
		}
	}

	if (direction == 'M' || direction == 'm')
	{
		every_thing_visible = (every_thing_visible == 1) ? 0 : 1;
		clear();
		refresh();
		return;
	}
	else if (direction == 'E' || direction == 'e')
	{
		eat_food(message);
		return;
	}
	else if (direction == 'I' || direction == 'i')
	{
		show_inventory();
		return;
	}
	else if (direction == 'g' || direction == 'G')
	{
		press_g = TRUE;
		while ((direction = getch()))
		{
			if (direction == 'f')
			{
				press_f = FALSE;
				break;
			}
			if (direction >= '1' && direction <= '9')
			{
				break;
			}
		}
	}

	if (last_pos == STAIR && (direction == KEY_RIGHT || direction == KEY_LEFT))
	{
		Cell c;
		c.x = *py;
		c.y = *px;
		if (direction == KEY_RIGHT && flooor != 0)
		{
			map[flooor][*py][*px] = last_pos;
			flooor--;
			c.flooor = flooor;
			map[flooor][*py][*px] = PLAYER;
			manage_monsters(c);
		}
		else if (direction == KEY_LEFT && flooor != FLOORS - 1)
		{
			// if (evey_rooms_in_this_floor_visited(flooor))
			if (1)
			{
				map[flooor][*py][*px] = last_pos;
				flooor++;
				c.flooor = flooor;
				map[flooor][*py][*px] = PLAYER;
				manage_monsters(c);
			}
			else
			{
				strcpy(message, "First explore all the rooms...");
				clear();
				refresh();
				return;
			}
		}
		rooms[flooor * ROOMS_PER_FLOOR].visited = 1;
		clear();
		refresh();
		return;
	}

	do
	{
		switch (direction)
		{
		case '7':
			new_y--;
			new_x--;
			break;
		case '8':
			new_y--;
			break;
		case '9':
			new_y--;
			new_x++;
			break;
		case '4':
			new_x--;
			break;
		case '6':
			new_x++;
			break;
		case '1':
			new_y++;
			new_x--;
			break;
		case '2':
			new_y++;
			break;
		case '3':
			new_y++;
			new_x++;
			break;
		default:
			return;
		}

		char next_cell = map[flooor][new_y][new_x];
		if (next_cell == MAGICAL_FOOD_IN_MAP || next_cell == GREAT_FOOD_IN_MAP ||
			next_cell == FLOOR || next_cell == DOOR || next_cell == CORRIDOR ||
			next_cell == STAIR || next_cell == TRAP || next_cell == GOLD_IN_MAP ||
			next_cell == BLACK_GOLD_IN_MAP || next_cell == SIMPLE_FOOD_IN_MAP ||
			next_cell == DAGGER_IN_MAP || next_cell == MAGIC_WAND_IN_MAP ||
			next_cell == NORMAL_ARROW_IN_MAP || next_cell == SWARD_IN_MAP ||
			next_cell == HEALTH_IN_MAP || next_cell == SPEED_IN_MAP || next_cell == DAMAGE_IN_MAP ||
			next_cell == FINISH_GAME_IN_MAP)
		{
			if (fullness >= FULLNESS_MAX)
			{
				hp += 3;
				if (hp > 100)
					hp = 100;
			}

			manage_foods();
			fullness--;
			if (fullness == 1)
			{
				strcpy(message, "You are very hungry 😢");
			}
			else if (!fullness)
			{
				hp -= 10;
				fullness = FULLNESS_MAX + 1;
			}

			Cell current_cell;
			current_cell.flooor = flooor;
			current_cell.x = *py;
			current_cell.y = *px;

			Cell c; // next cell
			c.flooor = flooor;
			c.x = new_y;
			c.y = new_x;

			map[flooor][*py][*px] = last_pos;
			*px = new_x;
			*py = new_y;
			last_pos = map[flooor][*py][*px];
			map[flooor][*py][*px] = PLAYER;
			stairs[flooor][*py][*px] = 1;

			if (get_room(c) != -1 && get_room(c) != get_room(current_cell))
			{
				reset_monsters_moves_in_room(c);
			}
			manage_monsters(c);

			int room_index = get_room(c);
			int nightmare = 0;
			if (room_index != -1 && rooms[room_index].kind == 3)
			{
				nightmare = 1;
			}

			if (next_cell == DOOR)
			{
				rooms[room_index].visited = 1;
			}
			else if (next_cell == GOLD_IN_MAP)
			{
				if (!press_g)
				{
					if (!nightmare)
					{
						gold += 2;
						strcpy(message, "You get 2 Gold 😀");
					}
					last_pos = FLOOR;
				}
				else
				{
					stairs[flooor][*py][*px] = 0;
				}
			}
			else if (next_cell == BLACK_GOLD_IN_MAP)
			{
				if (!press_g)
				{
					if (!nightmare)
					{
						strcpy(message, "You get 20 Gold 😃💪");
						gold += 20;
					}
					last_pos = FLOOR;
				}
				else
				{
					stairs[flooor][*py][*px] = 0;
				}
			}
			else if (next_cell == SIMPLE_FOOD_IN_MAP)
			{
				if (!press_g)
				{
					if (!nightmare)
					{
						int eat = 0;
						for (int i = 0; i < 5; i++)
						{
							if (!foods[i])
							{
								eat = 1;
								foods[i] = 1;
								foods_time[i] = 0;
								break;
							}
						}
						if (eat)
						{
							strcpy(message, "Ommmm, You get Simple Food 🍞");
						}
						else
						{
							strcpy(message, "Oh, The provision bag is full.");
						}
					}
					last_pos = FLOOR;
				}
				else
				{
					stairs[flooor][*py][*px] = 0;
				}
			}
			else if (next_cell == MAGICAL_FOOD_IN_MAP)
			{
				if (!press_g)
				{
					if (!nightmare)
					{
						int eat = 0;
						for (int i = 0; i < 5; i++)
						{
							if (!foods[i])
							{
								eat = 1;
								foods[i] = 3;
								foods_time[i] = 0;
								break;
							}
						}
						if (eat)
						{
							strcpy(message, "Ommmm, You get Magical Food 🧁🧙‍♀️");
						}
						else
						{
							strcpy(message, "Oh, The provision bag is full.");
						}
					}
					last_pos = FLOOR;
				}
				else
				{
					stairs[flooor][*py][*px] = 0;
				}
			}
			else if (next_cell == GREAT_FOOD_IN_MAP)
			{
				if (!press_g)
				{
					if (!nightmare)
					{
						int eat = 0;
						for (int i = 0; i < 5; i++)
						{
							if (!foods[i])
							{
								eat = 1;
								foods[i] = 2;
								foods_time[i] = 0;
								break;
							}
						}
						if (eat)
						{
							strcpy(message, "Ommmm, You get Great Food 🦐🍴");
						}
						else
						{
							strcpy(message, "Oh, The provision bag is full.");
						}
					}
					last_pos = FLOOR;
				}
				else
				{
					stairs[flooor][*py][*px] = 0;
				}
			}
			else if (next_cell == TRAP)
			{
				strcpy(message, "Ohhh, you went on TRAPE");
				hp -= 10;
			}
			else if (next_cell == DAGGER_IN_MAP)
			{
				if (!press_g)
				{
					if (!nightmare)
					{
						strcpy(message, "You get 10 Daggers 🗡");
						weapons[1] += 10;
					}
					last_pos = FLOOR;
				}
				else
				{
					stairs[flooor][*py][*px] = 0;
				}
			}
			else if (next_cell == MAGIC_WAND_IN_MAP)
			{
				if (!press_g)
				{
					if (!nightmare)
					{
						strcpy(message, "You get 8 Magic Wand 🪄");
						weapons[2] += 8;
					}
					last_pos = FLOOR;
				}
				else
				{
					stairs[flooor][*py][*px] = 0;
				}
			}
			else if (next_cell == NORMAL_ARROW_IN_MAP)
			{
				if (!press_g)
				{
					if (!nightmare)
					{
						strcpy(message, "You get 20 Noemal Arrows ➳");
						weapons[3] += 20;
					}
					last_pos = FLOOR;
				}
				else
				{
					stairs[flooor][*py][*px] = 0;
				}
			}
			else if (next_cell == SWARD_IN_MAP)
			{
				if (!press_g)
				{
					if (!nightmare)
					{
						strcpy(message, "You get Sward ⚔");
						weapons[4] = 1;
					}
					last_pos = FLOOR;
				}
				else
				{
					stairs[flooor][*py][*px] = 0;
				}
			}
			else if (next_cell == HEALTH_IN_MAP)
			{
				if (!press_g)
				{
					if (!nightmare)
					{
						strcpy(message, "You get Health potion 🩺");
						potions[0] += 1;
					}
					last_pos = FLOOR;
				}
				else
				{
					stairs[flooor][*py][*px] = 0;
				}
			}
			else if (next_cell == SPEED_IN_MAP)
			{
				if (!press_g)
				{
					if (!nightmare)
					{
						strcpy(message, "You get Speed potion 🚀");
						potions[1] += 1;
					}
					last_pos = FLOOR;
				}
				else
				{
					stairs[flooor][*py][*px] = 0;
				}
			}
			else if (next_cell == DAMAGE_IN_MAP)
			{
				if (!press_g)
				{
					if (!nightmare)
					{
						strcpy(message, "You get Damage potion 💥");
						potions[2] += 1;
					}
					last_pos = FLOOR;
				}
				else
				{
					stairs[flooor][*py][*px] = 0;
				}
			}
			else if (next_cell == FINISH_GAME_IN_MAP)
			{
				last_pos = FINISH_GAME_IN_MAP;
			}
		}
		else if (next_cell == WALL || next_cell == PILLAR || next_cell == WINDOWW)
		{
			return;
		}
		else
		{
			return;
		}

	} while (press_f);

	clear();
	refresh();
}

void start_playing()
{
	clear();
	initscr();
	noecho();
	cbreak();
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE);
	flooor = 0;
	generate_map();

	every_thing_visible = 0;
	int armor = 0; // زره
	int expp = 0;  // تجربه

	int px, py;

	for (int i = 0; i < WIDTH; i++)
	{
		for (int j = 0; j < HEIGHT; j++)
		{
			if (map[flooor][j][i] == PLAYER)
			{
				px = i;
				py = j;
			}
		}
	}

	rooms[0].visited = 1;

	print_map("Hello...");

	last_pos = FLOOR;
	int ch;
	char message[150];

	while (1)
	{
		if (hp <= 0)
		{
			finish_game();
			break;
		}
		ch = getch();
		if (ch != 'q' && ch != 'Q' && last_pos != FINISH_GAME_IN_MAP)
		{
			move_player(&px, &py, ch, message);
			print_map(message);
		}
		else
		{
			for (int i = 0; i < 150; i++)
			{
				message[i] = ' ';
			}
			int finish = 1;
			strcpy(message, "Save Game? (Y/N/Cancel)?");
			move(0, 0);
			print_map(message);

			while (1)
			{
				if (hp <= 0)
				{
					finish_game();
					finish = 1;
				}

				ch = getch();
				if (ch == 'y' || ch == 'Y')
				{
					/* code */
				}
				else if (ch == 'n' || ch == 'N')
				{
					finish = 1;
					break;
				}
				else if (ch == 'c' || ch == 'C')
				{
					for (int i = 0; i < 150; i++)
					{
						message[i] = ' ';
					}
					move(0, 0);
					print_map(message);
					break;
				}
			}
			if (finish)
			{
				break;
			}
		}
	}

	keypad(stdscr, TRUE);
	nodelay(stdscr, FALSE);
	clear();
	refresh();
	endwin();
}

Cell get_empty_cell(Room room)
{
	Cell empty_cells[room.width * room.height];
	int empty_cell_count = 0;

	for (int i = room.x + 1; i < room.x + room.width - 1; i++)
	{
		for (int j = room.y + 1; j < room.y + room.height - 1; j++)
		{
			if (map[room.flooor][i][j] == '.' && map[room.flooor][i][j - 1] == '.' && map[room.flooor][i][j + 1] == '.')
			{
				empty_cells[empty_cell_count].x = j;
				empty_cells[empty_cell_count].y = i;
				empty_cell_count++;
			}
		}
	}

	Cell c;

	if (empty_cell_count > 0)
	{
		int rand_index = rand() % empty_cell_count;
		c.x = empty_cells[rand_index].x;
		c.y = empty_cells[rand_index].y;
		c.flooor = room.flooor;
	}
	return c;
}

int get_room(Cell c)
{
	for (int i = 0; i < FLOORS * ROOMS_PER_FLOOR; i++)
	{
		if (c.flooor == rooms[i].flooor)
		{
			if ((c.x >= rooms[i].x && c.x < rooms[i].x + rooms[i].width) &&
				(c.y >= rooms[i].y && c.y < rooms[i].y + rooms[i].height))
			{
				return i;
			}
		}
	}
	return -1;
}

int show_stair(int x, int y)
{
	if (map[flooor][x][y] == CORRIDOR || map[flooor][x][y] == DOOR)
	{
		for (int i = -3; i < 4; i++)
		{
			for (int j = -3; j < 4; j++)
			{
				if (map[flooor][x + i][y + j] == PLAYER && (last_pos == DOOR || last_pos == CORRIDOR))
				{
					return 1;
				}
			}
		}
	}
	return 0;
}

int evey_rooms_in_this_floor_visited(int flooor)
{
	int visited = 1;
	for (int i = 0; i < ROOMS_PER_FLOOR; i++)
	{
		if (!rooms[flooor * ROOMS_PER_FLOOR + i].visited)
		{
			visited = 0;
			break;
		}
	}
	return visited;
}

void eat_food(char *message)
{
	clear();
	refresh();						// صفحه را پاک می‌کنیم
	int start_x = (COLS - 20) / 2;	// شروع در وسط صفحه افقی
	int start_y = (LINES - 10) / 2; // شروع در وسط صفحه عمودی

	// 1. نمایش نوار گرسنگی افقی
	move(LINES / 2 - 5, start_x);
	for (int i = 0; i < 10; i++) // نوار گرسنگی به‌صورت افقی
	{
		int per = fullness / 5;
		if (per > i)
		{
			printw("🟩"); // نمایش رنگ سبز برای سیری
		}
		else
		{
			printw("🟥"); // نمایش رنگ قرمز برای گرسنگی
		}
	}

	// 2. نمایش لیست غذاها
	move(LINES / 2 - 3, start_x); // موقعیت لیست غذاها
	for (int i = 0; i < 5; i++)
	{
		move(LINES / 2 - 3 + i, start_x);
		switch (foods[i])
		{
		case 0:
			printw("__");
			break;

		case 1:
			printw("%s", SIMPLE_FOOD);
			break;

		case 2:
			printw("%s", GREAT_FOOD);
			break;

		case 3:
			printw("%s", MAGICAL_FOOD);
			break;

		case 4:
			printw("%s", SIMPLE_FOOD);
			strcpy(message, "Oh, Your food was corrupt");
			break;

		default:
			break;
		}
	}

	// 3. درخواست ورودی از کاربر برای انتخاب غذا
	move(LINES - 2, start_x);
	printw("Press a number (1-5) to choose a food:");
	refresh();

	while (1)
	{
		int choice = getch() - '0'; // ورودی عددی برای انتخاب غذا
		if (choice >= 1 && choice <= 5)
		{
			if (foods[choice - 1] == 1)
			{
				foods[choice - 1] = 0;
				fullness = FULLNESS_MAX + 5;
			}
			else if (foods[choice - 1] == 2)
			{
				foods[choice - 1] = 0;
				fullness = FULLNESS_MAX + 5;
				power += 5;
			}
			else if (foods[choice - 1] == 3)
			{
				foods[choice - 1] = 0;
				fullness = FULLNESS_MAX + 5;
				speed += 5;
			}
			else
			{
				foods[choice - 1] = 0;
				fullness -= 6;
				if (fullness < 0)
				{
					fullness = 0;
				}
			}
			foods_time[choice - 1] = 0;
			break;
		}
		if (choice == 'q' - '0' || choice == 'Q' - '0')
		{
			break;
		}
	}

	// 5. برگشت به صفحه اصلی
	clear();
	refresh();
	getch(); // برای جلوگیری از بستن فوری صفحه
}

void show_inventory()
{
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);

	start_color();
	init_pair(1, COLOR_YELLOW, COLOR_BLACK);

	clear();
	refresh();

	int selected_potion = 0;
	int ch;

	mvprintw(1, (COLS - 20) / 2, "Your Inventory");
	mvprintw(15, (COLS - 60) / 2, "Press A-E to select weapon, arrow keys for potions, Q to quit");

	while (1)
	{
		// گروه اول: سلاح‌های 0 و 4
		attron(current_weapon == 0 ? COLOR_PAIR(1) : A_NORMAL);
		mvprintw(3, (COLS - 50) / 2, "[A] %-5s %s (%d) Power: %-3d Range: %-3d", weapons_icons[0], weapons_names[0], weapons[0], weapon_power[0], weapon_range[0]);
		attroff(current_weapon == 0 ? COLOR_PAIR(1) : A_NORMAL);

		attron(current_weapon == 4 ? COLOR_PAIR(1) : A_NORMAL);
		mvprintw(4, (COLS - 50) / 2, "[E] %-5s %s (%d) Power: %-3d Range: %-3d", weapons_icons[4], weapons_names[4], weapons[4], weapon_power[4], weapon_range[4]);
		attroff(current_weapon == 4 ? COLOR_PAIR(1) : A_NORMAL);

		// فاصله بین گروه‌ها
		mvprintw(5, (COLS - 30) / 2, "--------------------------");

		// گروه دوم: سلاح‌های 1، 2، 3
		attron(current_weapon == 1 ? COLOR_PAIR(1) : A_NORMAL);
		mvprintw(6, (COLS - 50) / 2, "[B] %-5s %s (%d) Power: %-3d Range: %-3d", weapons_icons[1], weapons_names[1], weapons[1], weapon_power[1], weapon_range[1]);
		attroff(current_weapon == 1 ? COLOR_PAIR(1) : A_NORMAL);

		attron(current_weapon == 2 ? COLOR_PAIR(1) : A_NORMAL);
		mvprintw(7, (COLS - 50) / 2, "[C] %-5s %s (%d) Power: %-3d Range: %-3d", weapons_icons[2], weapons_names[2], weapons[2], weapon_power[2], weapon_range[2]);
		attroff(current_weapon == 2 ? COLOR_PAIR(1) : A_NORMAL);

		attron(current_weapon == 3 ? COLOR_PAIR(1) : A_NORMAL);
		mvprintw(8, (COLS - 50) / 2, "[D] %-5s %s (%d) Power: %-3d Range: %-3d", weapons_icons[3], weapons_names[3], weapons[3], weapon_power[3], weapon_range[3]);
		attroff(current_weapon == 3 ? COLOR_PAIR(1) : A_NORMAL);
		// فاصله بین گروه‌ها
		mvprintw(9, (COLS - 30) / 2, "--------------------------");

		// نمایش معجون‌ها
		for (int i = 0; i < POTIONS_COUNT; i++)
		{
			if (i == selected_potion)
				attron(A_REVERSE);
			mvprintw(10 + i, (COLS - 30) / 2, "%s %s (%d)", potions_icons[i], potions_names[i], potions[i]);
			if (i == selected_potion)
				attroff(A_REVERSE);
		}

		// به‌روزرسانی صفحه
		refresh();

		// دریافت ورودی کاربر
		ch = getch();
		switch (ch)
		{
		case KEY_UP:
			if (selected_potion > 0)
				selected_potion--;
			break;
		case KEY_DOWN:
			if (selected_potion < POTIONS_COUNT - 1)
				selected_potion++;
			break;

		// انتخاب سلاح‌ها با کلیدهای خاص
		case 'a':
		case 'A':
			current_weapon = 0;
			break;
		case 'b':
		case 'B':
			current_weapon = 1;
			break;
		case 'c':
		case 'C':
			current_weapon = 2;
			break;
		case 'd':
		case 'D':
			current_weapon = 3;
			break;
		case 'e':
		case 'E':
			current_weapon = 4;
			break;

		// خروج
		case 'q':
		case 'Q':
			endwin();
			clear();
			refresh();
			return;
		}
	}
}

int show_nightmare(int x, int y)
{
	for (int i = -2; i < 3; i++)
	{
		for (int j = -2; j < 3; j++)
		{
			if (map[flooor][x + i][y + j] == PLAYER)
			{
				return 1;
			}
		}
	}
	return 0;
}

void manage_foods()
{
	for (int i = 0; i < 5; i++)
	{
		foods_time[i]++;
		if (foods_time[i] > 2 * FULLNESS_MAX)
		{
			foods_time[i] = 0;
			if (foods[i] == 2 || foods[i] == 3)
			{
				foods[i] = 1;
			}
			else if (foods[i] == 1)
			{
				foods[i] = 4;
			}
		}
	}
}

void center_text(int y, const char *text)
{
	int x = (COLS - strlen(text)) / 2;
	mvprintw(y, x, "%s", text);
}

void init_colors()
{
	start_color();
	init_pair(1, COLOR_RED, COLOR_BLACK);	 // رنگ قرمز
	init_pair(2, COLOR_GREEN, COLOR_BLACK);	 // رنگ سبز
	init_pair(3, COLOR_YELLOW, COLOR_BLACK); // رنگ زرد
}

void draw_happy_animation()
{
	const char *face = "   \\(^_^)/   ";
	const char *hand_up = "    /|\\    ";
	const char *hand_down = "    \\|/    ";

	for (int i = 0; i < 5; i++)
	{
		center_text(8, face);
		center_text(9, (i % 2 == 0) ? hand_up : hand_down);
		refresh();
		usleep(300000); // مکث 300 میلی‌ثانیه
	}
}

void draw_sad_animation()
{
	const char *face = "   (ಥ_ಥ)   ";
	const char *tear1 = "     💧    ";
	const char *tear2 = "    💧💧   ";
	const char *tear3 = "   💧💧💧  ";

	for (int i = 0; i < 8; i++)
	{
		center_text(8, face);
		switch (i % 4)
		{
		case 0:
			center_text(10, tear1);
			break;
		case 1:
			center_text(11, tear2);
			break;
		case 2:
			center_text(12, tear3);
			break;
		case 3:
			center_text(10, "                               ");
			center_text(11, "                               ");
			center_text(12, "                               ");
			center_text(8, face);
			break;
		}
		refresh();
		usleep(300000); // مکث 300 میلی‌ثانیه
	}
}

void finish_game()
{
	initscr();
	cbreak();
	noecho();
	curs_set(0);
	init_colors();

	clear();
	refresh();
	int score = 0;

	if (hp <= 0)
	{
		attron(COLOR_PAIR(1));
		center_text(3, "Game Over! You lost...");
		score = gold / 100;
		center_text(5, "Your Score:");
		mvprintw(6, (COLS / 2) - 2, "%d", score);
		attroff(COLOR_PAIR(1));
		refresh();
		draw_sad_animation();
	}
	else
	{
		attron(COLOR_PAIR(2));
		center_text(3, "Congratulations! You won!");
		score = hp + (gold * 10);
		center_text(5, "Your Score:");
		mvprintw(6, (COLS / 2) - 2, "%d", score);
		attroff(COLOR_PAIR(2));
		refresh();
		draw_happy_animation();
	}

	attron(COLOR_PAIR(3));
	center_text(LINES - 3, "Press Q to exit...");
	attroff(COLOR_PAIR(3));

	char ch;
	while (1)
	{
		ch = getch();
		if (ch == 'q' || ch == 'Q')
		{
			break;
		}
	}

	endwin();
}

void manage_monsters(Cell c)
{
	int room = get_room(c);
	int begin = 0;
	for (int i = 0; i < FLOORS * ROOMS_PER_FLOOR * 5; i++)
	{
		if (monsters[i].health > 0 && room == get_room(monsters[i].cell))
		{
			int monster_x = monsters[i].cell.x;
			int monster_y = monsters[i].cell.y;

			int move = 0;

			if (monsters[i].name == MONSTER_DEAMON && monsters[i].moves <= 4)
			{
				monsters[i].moves++;
				move = 1;
			}
			else if (monsters[i].name == MONSTER_FIRE_BREATHING)
			{
				if (monsters[i].moves <= 4)
				{
					monsters[i].moves++;
					move = 1;
				}
				else
				{
					int m_x = monster_x, m_y = monster_y;
					if (monster_x < c.x)
						m_x--;
					else if (monster_x > c.x)
						m_x++;

					if (monster_y < c.y)
						m_y--;
					else if (monster_y > c.y)
						m_y++;

					if (map[flooor][m_x][m_y] == FLOOR)
					{
						map[flooor][monsters[i].cell.x][monsters[i].cell.y] = FLOOR;
						monsters[i].cell.x = m_x;
						monsters[i].cell.y = m_y;
						map[flooor][m_x][m_y] = monsters[i].name;
						monster_x = m_x, monster_y = m_y;
					}
				}
			}
			else if (monsters[i].name == MONSTER_GIANT)
			{
				if (monsters[i].moves <= 4)
				{
					monsters[i].moves++;
					move = 1;
				}
				else
				{
					int m_x = monster_x, m_y = monster_y;
					if (monster_x < c.x)
						m_x--;
					else if (monster_x > c.x)
						m_x++;

					if (monster_y < c.y)
						m_y--;
					else if (monster_y > c.y)
						m_y++;

					if (map[flooor][m_x][m_y] == FLOOR)
					{
						map[flooor][monsters[i].cell.x][monsters[i].cell.y] = FLOOR;
						monsters[i].cell.x = m_x;
						monsters[i].cell.y = m_y;
						map[flooor][m_x][m_y] = monsters[i].name;
						monster_x = m_x, monster_y = m_y;
					}
				}
			}
			else if (monsters[i].name == MONSTER_SNAKE)
			{
				if (!monsters[i].moves)
				{
					monsters[i].moves = 1;
				}
			}
			else if (monsters[i].name == MONSTER_UNDEAD)
			{
				if (monsters[i].moves <= 5 && monsters[i].moves > 0)
				{
					monsters[i].moves++;
					move = 1;
				}
			}

			if (move)
			{
				int m_x = monster_x, m_y = monster_y;
				if (monster_x < c.x)
					m_x++;
				else if (monster_x > c.x)
					m_x--;

				if (monster_y < c.y)
					m_y++;
				else if (monster_y > c.y)
					m_y--;

				if (map[flooor][m_x][m_y] == FLOOR)
				{
					map[flooor][monsters[i].cell.x][monsters[i].cell.y] = FLOOR;
					monsters[i].cell.x = m_x;
					monsters[i].cell.y = m_y;
					map[flooor][m_x][m_y] = monsters[i].name;
					monster_x = m_x, monster_y = m_y;
				}
			}

			if (abs(monster_x - c.x) <= 1 && abs(monster_y - c.y) <= 1)
			{
				enum MonsterPower monster_power;
				switch (monsters[i].name)
				{
				case MONSTER_DEAMON:
					monster_power = D;
					break;
				case MONSTER_FIRE_BREATHING:
					monster_power = F;
					break;
				case MONSTER_GIANT:
					monster_power = G;
					break;
				case MONSTER_SNAKE:
					monster_power = S;
					break;
				case MONSTER_UNDEAD:
				{
					monster_power = U;
					if (monsters[i].moves == 0)
						monsters[i].moves = 1;
					break;
				}

				default:
					break;
				}
				hp -= monster_power;
				char message[200];
				sprintf(message, "You lost %d points in the battle with the %c", monster_power, monsters[i].name);
				move(0, 0);
				print_map(message);
				usleep(1000000);
				return;
			}
		}

		if (monsters[i].health > 0 && monsters[i].moves && monsters[i].cell.flooor == flooor)
		{
			int monster_x = monsters[i].cell.x;
			int monster_y = monsters[i].cell.y;

			int m_x = monster_x, m_y = monster_y;
			if (monster_x < c.x)
				m_x++;
			else if (monster_x > c.x)
				m_x--;

			if (monster_y < c.y)
				m_y++;
			else if (monster_y > c.y)
				m_y--;

			if (map[flooor][m_x][m_y] == FLOOR ||
				map[flooor][m_x][m_y] == DOOR ||
				map[flooor][m_x][m_y] == CORRIDOR)
			{
				map[flooor][monsters[i].cell.x][monsters[i].cell.y] = monsters[i].last_pos;
				monsters[i].cell.x = m_x;
				monsters[i].cell.y = m_y;
				monsters[i].last_pos = map[flooor][m_x][m_y];
				map[flooor][m_x][m_y] = monsters[i].name;
				monster_x = m_x, monster_y = m_y;
			}

			if (abs(monster_x - c.x) <= 1 && abs(monster_y - c.y) <= 1)
			{
				enum MonsterPower monster_power = S;
				hp -= monster_power;
				char message[200];
				sprintf(message, "You lost %d points in the battle with the %c", monster_power, monsters[i].name);
				move(0, 0);
				print_map(message);
				usleep(1000000);
				return;
			}
		}
	}
}

void add_monster(Monster m)
{
	static int i = 0;
	monsters[i] = m;
	i++;
}

void reset_monsters_moves_in_room(Cell cell)
{
	int room_index = get_room(cell);

	if (room_index == -1)
	{
		return;
	}

	for (int i = 0; i < FLOORS * ROOMS_PER_FLOOR * 5; i++)
	{
		if (monsters[i].health > 0)
		{
			int monster_room_index = get_room(monsters[i].cell);

			if (monster_room_index == room_index)
			{
				monsters[i].moves = 0;
			}
		}
	}
}
