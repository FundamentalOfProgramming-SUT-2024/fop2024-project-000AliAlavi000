#include <stdio.h>
#include <sqlite3.h>
#include <string.h>

// تابع برای چاپ پیام‌های خطا
void print_error(const char *message)
{
    fprintf(stderr, "Error: %s\n", message);
}

// تابع برای اتصال به پایگاه داده
sqlite3 *connect_to_database(const char *db_name)
{
    sqlite3 *db;
    int rc = sqlite3_open(db_name, &db);
    if (rc != SQLITE_OK)
    {
        print_error(sqlite3_errmsg(db));
        return NULL;
    }
    return db;
}

// تابع برای نمایش اطلاعات به شکل جدول
int display_users_table(sqlite3 *db)
{
    const char *query = "SELECT ID, Username, Email, Level, Color, Song, LastPos, Floor, EverythingVisible, Fullness, CurrentWeapon, SpeedMoving, HP, Gold, RecoveryHealth FROM Users;";
    sqlite3_stmt *stmt;

    // آماده‌سازی کوئری
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        print_error(sqlite3_errmsg(db));
        return -1;
    }

    // چاپ هدر جدول
    printf("---------------------------------------------------------------------------------------------------------------------------------\n");
    printf("| %-3s | %-15s | %-20s | %-5s | %-5s | %-4s | %-7s | %-5s | %-16s | %-8s | %-13s | %-12s | %-3s | %-4s | %-15s |\n",
           "ID", "Username", "Email", "Level", "Color", "Song", "LastPos", "Floor", "EverythingVisible", "Fullness", "CurrentWeapon", "SpeedMoving", "HP", "Gold", "RecoveryHealth");
    printf("---------------------------------------------------------------------------------------------------------------------------------\n");

    // چاپ داده‌ها
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        int id = sqlite3_column_int(stmt, 0);
        const char *username = (const char *)sqlite3_column_text(stmt, 1);
        const char *email = (const char *)sqlite3_column_text(stmt, 2);
        int level = sqlite3_column_int(stmt, 3);
        int color = sqlite3_column_int(stmt, 4);
        int song = sqlite3_column_int(stmt, 5);
        const char *last_pos = (const char *)sqlite3_column_text(stmt, 6);
        int floor = sqlite3_column_int(stmt, 7);
        int everything_visible = sqlite3_column_int(stmt, 8);
        int fullness = sqlite3_column_int(stmt, 9);
        int current_weapon = sqlite3_column_int(stmt, 10);
        int speed_moving = sqlite3_column_int(stmt, 11);
        int hp = sqlite3_column_int(stmt, 12);
        int gold = sqlite3_column_int(stmt, 13);
        int recovery_health = sqlite3_column_int(stmt, 14);

        printf("| %-3d | %-15s | %-20s | %-5d | %-5d | %-4d | %-7s | %-5d | %-16d | %-8d | %-13d | %-12d | %-3d | %-4d | %-15d |\n",
               id, username, email, level, color, song, last_pos, floor, everything_visible, fullness, current_weapon, speed_moving, hp, gold, recovery_health);
    }
    printf("---------------------------------------------------------------------------------------------------------------------------------\n");

    // آزاد کردن منابع
    sqlite3_finalize(stmt);
    return 0;
}

// تابع اصلی
int main()
{
    const char *db_name = "game.db";
    sqlite3 *db = connect_to_database(db_name);
    if (!db)
    {
        return 1;
    }

    // نمایش اطلاعات جدول Users
    if (display_users_table(db) != 0)
    {
        sqlite3_close(db);
        return 1;
    }

    // بستن اتصال به پایگاه داده
    sqlite3_close(db);
    return 0;
}