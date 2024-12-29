#include <stdio.h>
#include <stdlib.h>

// تابع برای پخش فایل MP3
void play_mp3(const char *file_path)
{
    char command[256];

    // ساخت دستور برای پخش MP3
    snprintf(command, sizeof(command), "mpg123 %s > /dev/null 2>&1 &", file_path);

    // اجرای دستور
    int result = system(command);
    if (result == -1)
    {
        printf("Failed to play the file. Please make sure mpg123 is installed.\n");
    }
    else
    {
        printf("Playing: %s\n", file_path);
    }
}

int main()
{
    // مسیر فایل MP3
    play_mp3("1.mp3");

    // جلوگیری از خروج برنامه
    printf("Press ENTER to stop the program.\n");
    getchar();

    return 0;
}