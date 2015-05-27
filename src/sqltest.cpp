#include "sqltest.h"

#include <QDebug>
#include <sqlite3.h>

void test_sql()
{
  sqlite3 *db;
/*  char *err_msg = 0;*/
  sqlite3_stmt *res;

  int rc = sqlite3_open("test.db", &db);
  if (rc != SQLITE_OK) {
    qCritical() << "Cannot open database:" << sqlite3_errmsg(db);
    sqlite3_close(db);
    return;
  }

  const char *const sql = "SELECT id, name FROM Cats WHERE shop = ? and price > NULL";
  rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
  if (rc == SQLITE_OK) {
    sqlite3_bind_text(res, 1, "Shop2", 5, SQLITE_STATIC);
/*    sqlite3_bind_int(res, 2, 1900);*/
  } else {
      fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
  }

  int step;/* = sqlite3_step(res);*/
  do {
    step = sqlite3_step(res);
    if (step != SQLITE_ROW) {
      qDebug() << "Step result:" << step;
      break;
    }
    printf("%s: ", sqlite3_column_text(res, 0));
    printf("%s\n", sqlite3_column_text(res, 1));
  } while (true);

  sqlite3_finalize(res);
  sqlite3_close(db);
}
