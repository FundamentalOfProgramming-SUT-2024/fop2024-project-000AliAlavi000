#include <ctype.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#define FILENAME "users.txt"

char user_name[50];
int level = 0; // 1:easy 2:medium 3:hard;
int color = 0; // 1:red 2:green 3:blue;
int song = 0;

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

	fprintf(file, "%s %s %s\n", username, password, email);
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

	if (song == 0)
	{
		return;
	}

	snprintf(command, sizeof(command), "pkill mpg123");
	system(command);

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


