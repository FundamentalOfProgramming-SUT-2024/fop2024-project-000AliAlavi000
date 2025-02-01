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

// تابع برای اجرای کوئری‌های ساده
int execute_query(sqlite3 *db, const char *query)
{
    char *err_msg = 0;
    int rc = sqlite3_exec(db, query, 0, 0, &err_msg);
    if (rc != SQLITE_OK)
    {
        print_error(err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
    return 0;
}

// تابع برای ایجاد جدول مخاطبین
int create_contacts_table(sqlite3 *db)
{
    const char *query = "CREATE TABLE IF NOT EXISTS Contacts ("
                        "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                        "Name TEXT NOT NULL, "
                        "Phone TEXT NOT NULL);";
    return execute_query(db, query);
}

// تابع برای افزودن مخاطب جدید
int add_contact(sqlite3 *db, const char *name, const char *phone)
{
    char query[256];
    snprintf(query, sizeof(query), "INSERT INTO Contacts (Name, Phone) VALUES ('%s', '%s');", name, phone);
    return execute_query(db, query);
}

// تابع برای نمایش تمام مخاطبین
int display_contacts(sqlite3 *db)
{
    const char *query = "SELECT * FROM Contacts;";
    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        print_error(sqlite3_errmsg(db));
        return -1;
    }

    printf("Contacts:\n");
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        int id = sqlite3_column_int(stmt, 0);
        const char *name = (const char *)sqlite3_column_text(stmt, 1);
        const char *phone = (const char *)sqlite3_column_text(stmt, 2);
        printf("ID: %d, Name: %s, Phone: %s\n", id, name, phone);
    }

    sqlite3_finalize(stmt);
    return 0;
}

// تابع اصلی
int main()
{
    const char *db_name = "phonebook.db";
    sqlite3 *db = connect_to_database(db_name);
    if (!db)
    {
        return 1;
    }

    // ایجاد جدول مخاطبین
    if (create_contacts_table(db) != 0)
    {
        sqlite3_close(db);
        return 1;
    }

    // افزودن چند مخاطب نمونه
    add_contact(db, "Ali", "123456789");
    add_contact(db, "Reza", "987654321");

    // نمایش مخاطبین
    display_contacts(db);

    // بستن اتصال به پایگاه داده
    sqlite3_close(db);
    return 0;
}