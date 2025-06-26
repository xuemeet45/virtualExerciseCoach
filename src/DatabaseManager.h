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
    bool insert_exercise_pose(int exercise_id, int keypoint_index, float x, float y, float confidence, int frame_number);

private:
    PGconn *conn;
};

extern DatabaseManager* global_db_manager;

#endif // DATABASEMANAGER_H
