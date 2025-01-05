#include <ncurses.h>
#include <stdbool.h>
#include <unistd.h> // برای usleep

#define TIMEOUT 200000 // 200 میلی‌ثانیه برای بررسی همزمانی

int main()
{
    initscr();
    noecho();
    cbreak();
    nodelay(stdscr, TRUE);

    bool f_pressed = false;
    bool j_pressed = false;

    while (true)
    {
        int ch = getch();

        // اگر کاربر کلید q بزند، خارج شویم
        if (ch == 'q')
            break;

        // بررسی کلید f
        if (ch == 'f')
        {
            f_pressed = true;
            usleep(TIMEOUT); // مکث کوتاه برای بررسی همزمانی
        }

        // بررسی کلید j
        if (ch == 'j')
        {
            j_pressed = true;
            usleep(TIMEOUT); // مکث کوتاه برای بررسی همزمانی
        }

        // اگر هر دو کلید f و j همزمان فشرده شده باشند
        if (f_pressed && j_pressed)
        {
            mvprintw(0, 0, "Hello, User!");
            refresh();
            break;
        }

        // بازنشانی وضعیت کلیدها
        f_pressed = false;
        j_pressed = false;
    }

    endwin();
    return 0;
}
