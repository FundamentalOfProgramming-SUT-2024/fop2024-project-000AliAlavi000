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
#define GOLD "☀"
#define GOLD_IN_MAP 'g'
#define BLACK_GOLD_IN_MAP 'G'

// format: username password email scores golds finish_games time_left
char user_name[50];
int level = 1; // 1:easy 2:medium 3:hard;
int color = 0; // 1:red 2:green 3:blue;
int song = 0;  // 0:none 1:moosh 2: ali
char last_pos;
int flooor = 0;
int every_thing_visible = 0;

typedef struct
{
	int flooor, x, y;  // مختصات شروع زیرجدول
	int width, height; // ابعاد زیرجدول
	int visited;	   // 1:visited 0:
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

struct User
{
	char username[50];
	int scores;
	int golds;
	int finish_games;
	int time_left;
};

char map[FLOORS][WIDTH][HEIGHT];
Room rooms[FLOORS * ROOMS_PER_FLOOR];
int stairs[FLOORS][WIDTH][HEIGHT];

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
void print_map(char message[], int gold, int hp);
void move_player(int *px, int *py, int direction, char *message, int *hp, int *gold);
void start_playing();
int contains(int num, int nums[], int size);
Cell get_empty_cell(Room room);
int get_room(Cell c);
int show_stair(int x, int y);
int evey_rooms_in_this_floor_visited(int flooor);

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

	char *choices[] = {"1. Create New User", "2. Login",
					   "3. Start Playing", "4. Profile",
					   "5. Scores Table", "6. Setting", "7. Exit"};
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
			// Start Playing
			start_playing();
		}
		else if (choice == 5)
		{
			// scores
			scores_table();
		}
		else if (choice == 6)
		{
			game_setting();
			play_mp3();
			// setting
		}
		else if (choice == 7)
		{
			break; // Exit
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

	if (guest[0] == 'Y')
	{
		guest_login();
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

	if (forgot[0] == 'Y')
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
				strcat(users[i].username, " (goat)");
			}
			else if (i == 1)
			{
				strcpy(rank_display, "🥈   ");
				strcat(users[i].username, " (master)");
			}
			else if (i == 2)
			{
				strcat(users[i].username, " (king)");
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

				int pillars = rand() % 4;
				int traps = rand() % (8 - level);
				int gold = rand() % (4 + level);
				int black_gold = rand() % (12 + (2 * level));
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
				if (!traps)
				{
					Cell t = get_empty_cell(new_room);
					map[floor][t.y][t.x] = TRAP;
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
		connect_rooms(floor);
	}
	Cell stair = get_empty_cell(rooms[0]);
	for (int i = 0; i < FLOORS; i++)
	{
		map[i][stair.y][stair.x] = STAIR;
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

void print_map(char message[], int gold, int hp)
{
	start_color();
	init_pair(1, COLOR_YELLOW, COLOR_BLACK);
	init_pair(2, COLOR_WHITE, COLOR_BLACK);
	init_pair(3, COLOR_RED, COLOR_BLACK);
	init_pair(4, COLOR_GREEN, COLOR_BLACK);
	init_pair(5, COLOR_BLACK, COLOR_WHITE);
	init_pair(6, COLOR_BLACK, COLOR_YELLOW);

	printw("%s", message);

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
			if (stairs[flooor][i][j] || rooms[get_room(c)].visited || every_thing_visible || show_stair(i, j))
			{
				if (map[flooor][i][j] == TRAP && !stairs[flooor][i][j])
				{
					// attron(COLOR_PAIR(3));
					mvprintw(i + start_x, j + start_y, "%c", FLOOR);
					// attroff(COLOR_PAIR(3));
				}
				else if (map[flooor][i][j] == GOLD_IN_MAP && !stairs[flooor][i][j])
				{
					attron(COLOR_PAIR(1));
					mvprintw(i + start_x, j + start_y, "%s", GOLD);
					attroff(COLOR_PAIR(1));
				}
				else if (map[flooor][i][j] == BLACK_GOLD_IN_MAP && !stairs[flooor][i][j])
				{
					attron(COLOR_PAIR(6));
					mvprintw(i + start_x, j + start_y, "%s", GOLD);
					attroff(COLOR_PAIR(6));
				}
				else
				{
					mvprintw(i + start_x, j + start_y, "%c", map[flooor][i][j]);
				}
			}
		}
	}

	move(LINES - 1, 0);
	printw("Floor: %d\tHP: %d\tGold: %d", flooor + 1, hp, gold);
	refresh();
}

void move_player(int *px, int *py, int direction, char *message, int *hp, int *gold)
{
	for (int i = 0; i < 150; i++)
	{
		message[i] = ' ';
	}
	int new_x = *px;
	int new_y = *py;
	int press_f = FALSE;
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

	if (last_pos == STAIR && (direction == KEY_RIGHT || direction == KEY_LEFT))
	{
		if (direction == KEY_RIGHT && flooor != 0)
		{
			map[flooor][*py][*px] = last_pos;
			flooor--;
			map[flooor][*py][*px] = PLAYER;
		}
		else if (direction == KEY_LEFT && flooor != FLOORS - 1)
		{
			// if (evey_rooms_in_this_floor_visited(flooor))
			if (1)
			{
				map[flooor][*py][*px] = last_pos;
				flooor++;
				map[flooor][*py][*px] = PLAYER;
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
		if (next_cell == FLOOR || next_cell == DOOR || next_cell == CORRIDOR || next_cell == STAIR || next_cell == TRAP || next_cell == GOLD_IN_MAP || next_cell == BLACK_GOLD_IN_MAP)
		{
			map[flooor][*py][*px] = last_pos;
			*px = new_x;
			*py = new_y;
			last_pos = map[flooor][*py][*px];
			map[flooor][*py][*px] = PLAYER;
			stairs[flooor][*py][*px] = 1;
			if (next_cell == DOOR)
			{
				Cell c;
				c.flooor = flooor;
				c.x = new_y;
				c.y = new_x;
				rooms[get_room(c)].visited = 1;
			}
			else if (next_cell == GOLD_IN_MAP)
			{
				strcpy(message, "You get 2 Gold 😀");
				*gold += 2;
				last_pos = FLOOR;
			}
			else if (next_cell == BLACK_GOLD_IN_MAP)
			{
				strcpy(message, "You get 20 Gold 😃💪");
				*gold += 20;
				last_pos = FLOOR;
			}
			else if (next_cell == TRAP)
			{
				strcpy(message, "Ohhh, you went on TRAPE");
				*hp -= 10;
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

	int gold = 0;  // طلا
	int armor = 0; // زره
	int expp = 0;  // تجربه
	int hp = 100;  // جان

	Cell c = get_empty_cell(rooms[0]);
	int px = c.x, py = c.y; // موقعیت اولیه بازیکن
	rooms[0].visited = 1;
	map[0][py][px] = PLAYER;

	print_map("Hello...", gold, hp);

	last_pos = FLOOR;
	int ch;
	char message[150];

	while ((ch = getch()) != 'q')
	{
		move_player(&px, &py, ch, message, &hp, &gold);
		print_map(message, gold, hp);
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
			if (map[room.flooor][i][j] == '.')
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