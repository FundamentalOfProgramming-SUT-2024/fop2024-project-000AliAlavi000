#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>

#define FILENAME "users.txt"

void add_new_user(const char* message);
int validate_username(const char *username);
int validate_password(const char *password);
int validate_email(const char *email);
int contain_space(const char *text);
char* generate_password(int length);
void save_user(const char *username, const char *password, const char *email);

int main() {
    initscr();
    clear();
    noecho();
    cbreak();
    curs_set(0);

    int startx = 0;
    int starty = 0;

    const char *choices[] = {
        "1. Create New User",
        "2. Login",
        "3. Start Playing",
        "4. Profile",
        "5. Scores Table",
        "6. Exit"
    };
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

    while (1) {
        box(menu_win, 0, 0);
        for (int i = 0; i < n_choices; ++i) {
            if (highlight == i + 1) {
                wattron(menu_win, A_REVERSE);
                mvwprintw(menu_win, i + 1, 2, "%s", choices[i]);
                wattroff(menu_win, A_REVERSE);
            } else {
                mvwprintw(menu_win, i + 1, 2, "%s", choices[i]);
            }
        }
        wrefresh(menu_win);
        c = wgetch(menu_win);

        switch (c) {
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

        if (choice == 1) {
            endwin();
            add_new_user("");
            initscr();
            clear();
        } else if (choice == n_choices) {
            break;
        }
        choice = 0;
    }

    endwin();
    return 0;
}

void add_new_user(const char* message) {
    char username[50], password[50], email[100];

    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLACK); 
    init_pair(2, COLOR_WHITE, COLOR_BLACK); 
    init_pair(3, COLOR_RED, COLOR_BLACK);  
    init_pair(4, COLOR_GREEN, COLOR_BLACK); 

    echo();
    curs_set(1);
    clear(); 

    if(strlen(message) > 2) {
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

    if (!validate_username(username)) {
        add_new_user("\nUsername is already registered or contains whitespace.");
        return;
    }

    attron(COLOR_PAIR(1)); 
    printw("\nPassword:");
    attroff(COLOR_PAIR(1)); 
    attron(COLOR_PAIR(2)); 
    getstr(password);
    attroff(COLOR_PAIR(2)); 

    if (!validate_password(password)) {
        add_new_user("\nPassword should contain at least 7 chars and lower and upper and number or contains whitespace.");
        return;
    }

    attron(COLOR_PAIR(1)); 
    printw("\nEmail: ");
    attroff(COLOR_PAIR(1));
    attron(COLOR_PAIR(2));
    getstr(email);
    attroff(COLOR_PAIR(2)); 
    if (!validate_email(email)) {
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

int validate_username(const char *username) {
    if(contain_space(username)){
        return 0;
    }

    FILE *file = fopen(FILENAME, "r");
    if (!file) return 1;

    char line[200];
    while (fgets(line, sizeof(line), file)) {
        char stored_username[50];
        sscanf(line, "%49s", stored_username);
        if (strcmp(username, stored_username) == 0) {
            fclose(file);
            return 0;
        }
    }

    fclose(file);
    return 1;
}

int validate_password(const char *password) {
    if(contain_space(password)){
        return 0;
    }

    if (strlen(password) < 7) return 0;

    int has_upper = 0, has_lower = 0, has_digit = 0;
    for (int i = 0; password[i] != '\0'; i++) {
        if (isupper(password[i])) has_upper = 1;
        if (islower(password[i])) has_lower = 1;
        if (isdigit(password[i])) has_digit = 1;
    }

    return has_upper && has_lower && has_digit;
}

int validate_email(const char *email) {
    if(contain_space(email)){
        return 0;
    }

    const char *at_ptr = strchr(email, '@');
    if (!at_ptr) return 0;

    const char *dot_ptr = strchr(at_ptr, '.');
    if (!dot_ptr || dot_ptr == at_ptr + 1 || dot_ptr[1] == '\0') return 0;

    return 1;
}

int contain_space(const char* text) {
    if(strchr(text, ' ') == NULL){
        return 0;
    }
    return 1;
}

char* generate_password(int length) {
    static char password[100]; 
    char charset_upper[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char charset_lower[] = "abcdefghijklmnopqrstuvwxyz";
    char charset_digits[] = "0123456789";
    char charset_all[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    srand(time(NULL));

    password[0] = charset_upper[rand() % strlen(charset_upper)];
    password[1] = charset_lower[rand() % strlen(charset_lower)];
    password[2] = charset_digits[rand() % strlen(charset_digits)];

    for (int i = 3; i < length; i++) {
        password[i] = charset_all[rand() % strlen(charset_all)];
    }

    for (int i = 0; i < length; i++) {
        int j = rand() % length;
        char temp = password[i];
        password[i] = password[j];
        password[j] = temp;
    }

    password[length] = '\0';
    return password;
}

void save_user(const char *username, const char *password, const char *email) {
    FILE *file = fopen(FILENAME, "a");
    if (!file) {
        add_new_user("\nTry again please...");
        return;
    }

    fprintf(file, "%s %s %s\n", username, password, email);
    fclose(file);
}