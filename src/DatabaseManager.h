#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <libpq-fe.h>
#include <vector>
#include <string>
#include <glibmm/ustring.h>
#include "Exercise.h"

class DatabaseManager {
public:
    DatabaseManager(const std::string& conninfo);
    ~DatabaseManager();

    std::vector<Exercise> fetch_exercises(const std::string& query);

private:
    PGconn *conn;
};

extern DatabaseManager* global_db_manager;

#endif // DATABASEMANAGER_H
