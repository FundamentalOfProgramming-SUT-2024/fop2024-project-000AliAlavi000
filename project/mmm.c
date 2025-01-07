#include <ncurses.h>
#include <locale.h>

int main()
{
    // تنظیم locale برای پشتیبانی از UTF-8
    setlocale(LC_ALL, "");

    // شروع ncurses
    initscr();

    // استفاده از تابع wide برای چاپ رشته‌های یونیکد
    printw("سلام دنیا! 🌍\n");
    //addwstr(L"Unicode Test: 😊🚀🌟\n");

    // منتظر کلید بمانید
    refresh();
    getch();

    // پایان ncurses
    endwin();

    return 0;
}