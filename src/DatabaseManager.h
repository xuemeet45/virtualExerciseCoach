#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <libpq-fe.h>
#include <vector>
#include <string>
#include <glibmm/ustring.h>
#include <map>
#include "Exercise.h"
#include "User.h"

class DatabaseManager {
public:
    DatabaseManager(const std::string& conninfo);
    ~DatabaseManager();

    std::vector<Exercise> fetch_exercises(const std::string& query);
    bool insert_exercise_pose(int exercise_id, int keypoint_index, float x, float y, float confidence, int frame_number);
    bool insert_reference_pose(int exercise_id, int keypoint_index, float x, float y, float confidence, int frame_number = 0);
    std::vector<std::array<float, 3>> fetch_reference_pose(int exercise_id);
    
    // User management methods
    std::vector<User> fetch_users(const std::string& query);
    bool execute_query(const std::string& query);
    std::vector<std::map<std::string, std::string>> fetch_sessions(const std::string& query);
    std::vector<std::map<std::string, std::string>> fetch_statistics(const std::string& query);
    bool update_user_profile(const User& user);

    // New methods for exercise history
    int insert_exercise_session_start(int user_id, int exercise_id, const std::string& status = "tried");
    bool update_exercise_session_end(int history_id, const std::string& status, int performed_seconds, int calories_burned, int duration_minutes, const std::string& notes = "");
    std::vector<std::map<std::string, std::string>> fetch_exercise_history(int user_id);

private:
    PGconn *conn;
};

extern DatabaseManager* global_db_manager;

#endif // DATABASEMANAGER_H
