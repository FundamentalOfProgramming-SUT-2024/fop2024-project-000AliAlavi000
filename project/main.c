#include <ctype.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#define FILENAME "users.txt"
#define ROWS_PER_PAGE 20

// format: username password email scores golds finish_games time_left
char user_name[50];
int level = 0; // 1:easy 2:medium 3:hard;
int color = 0; // 1:red 2:green 3:blue;
int song = 0;

struct User
{
	char username[50];
	int scores;
	int golds;
	int finish_games;
	int time_left;
};

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

int main()
{
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
				strcpy(rank_display, "🥇");
			else if (i == 1)
				strcpy(rank_display, "🥈");
			else if (i == 2)
				strcpy(rank_display, "🥉");
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

	endwin();
}
