#include "DatabaseManager.h"
#include <iostream>
#include <sstream>
#include <array>

DatabaseManager* global_db_manager = nullptr;

DatabaseManager::DatabaseManager(const std::string& conninfo) {
    conn = PQconnectdb(conninfo.c_str());

    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Connection to database failed: " << PQerrorMessage(conn) << std::endl;
        conn = nullptr;
    }
}

DatabaseManager::~DatabaseManager() {
    if (conn) {
        PQfinish(conn);
    }
}

std::vector<Exercise> DatabaseManager::fetch_exercises(const std::string& query) {
    if (!conn) {
        return {};
    }

    PGresult *res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Query failed: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        return {};
    }

    std::vector<Exercise> exercises;
    int nRows = PQntuples(res);

    for (int i = 0; i < nRows; i++) {
        int id = std::stoi(PQgetvalue(res, i, 0));
        Glib::ustring name = PQgetvalue(res, i, 1);
        Glib::ustring image_path = PQgetvalue(res, i, 2);
        Glib::ustring category = PQgetvalue(res, i, 3);
        Glib::ustring primary_muscle = PQgetvalue(res, i, 4);
        Glib::ustring secondary_muscles = PQgetvalue(res, i, 5);
        Glib::ustring equipment = PQgetvalue(res, i, 6);
        Glib::ustring difficulty_level = PQgetvalue(res, i, 7);
        Glib::ustring instructions = PQgetvalue(res, i, 8);
        Glib::ustring tips = PQgetvalue(res, i, 9);
        Glib::ustring video_url = PQgetvalue(res, i, 10);
        Glib::ustring reps_sets_suggestion = PQgetvalue(res, i, 11);
        Glib::ustring benefits = PQgetvalue(res, i, 12);
        Glib::ustring common_mistakes = PQgetvalue(res, i, 13);
        Glib::ustring variations = PQgetvalue(res, i, 14);
        Glib::ustring calories_burned_estimate_str = PQgetvalue(res, i, 15);
        Glib::ustring updated_at = PQgetvalue(res, i, 16);

        // Parse calories_burned_estimate from string to int
        int calories_burned_estimate_int = 0;
        try {
            std::string s = calories_burned_estimate_str.raw();
            // Remove " calories/minute" if present
            size_t pos = s.find(" calories/minute");
            if (pos != std::string::npos) {
                s = s.substr(0, pos);
            }
            calories_burned_estimate_int = std::stoi(s);
        } catch (const std::exception& e) {
            std::cerr << "Error parsing calories_burned_estimate: " << e.what() << " for value: " << calories_burned_estimate_str << std::endl;
            // Default to 0 if parsing fails
            calories_burned_estimate_int = 0;
        }

        exercises.emplace_back(id, name, image_path, category, primary_muscle, secondary_muscles,
                               equipment, difficulty_level, instructions, tips, video_url,
                               reps_sets_suggestion, benefits, common_mistakes, variations,
                               calories_burned_estimate_int, updated_at);
    }

    PQclear(res);
    return exercises;
}

bool DatabaseManager::insert_exercise_pose(int exercise_id, int keypoint_index, float x, float y, float confidence, int frame_number) {
    if (!conn) return false;
    std::ostringstream oss;
    oss << "INSERT INTO exercise_poses (exercise_id, keypoint_index, x, y, confidence, frame_number) VALUES ("
        << exercise_id << ", " << keypoint_index << ", " << x << ", " << y << ", " << confidence << ", " << frame_number << ");";
    PGresult* res = PQexec(conn, oss.str().c_str());
    bool success = PQresultStatus(res) == PGRES_COMMAND_OK;
    if (!success) {
        std::cerr << "Failed to insert pose data: " << PQerrorMessage(conn) << std::endl;
    }
    PQclear(res);
    return success;
}

int DatabaseManager::insert_exercise_session_start(int user_id, int exercise_id, const std::string& status) {
    if (!conn) return -1; // Return -1 to indicate failure

    std::ostringstream oss;
    oss << "INSERT INTO exercise_history (user_id, exercise_id, status) VALUES ("
        << user_id << ", " << exercise_id << ", '" << status << "') RETURNING id;";
    
    PGresult* res = PQexec(conn, oss.str().c_str());
    int history_id = -1;

    if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) > 0) {
        history_id = std::stoi(PQgetvalue(res, 0, 0));
    } else {
        std::cerr << "Failed to insert exercise session start: " << PQerrorMessage(conn) << std::endl;
    }
    PQclear(res);
    return history_id;
}

bool DatabaseManager::update_exercise_session_end(int history_id, const std::string& status, int performed_seconds, int calories_burned, int duration_minutes, const std::string& notes) {
    if (!conn) return false;

    std::ostringstream oss;
    oss << "UPDATE exercise_history SET "
        << "status = '" << status << "', "
        << "performed_seconds = " << performed_seconds << ", "
        << "calories_burned = " << calories_burned << ", "
        << "duration_minutes = " << duration_minutes << ", "
        << "notes = '" << notes << "' "
        << "WHERE id = " << history_id << ";";
    
    PGresult* res = PQexec(conn, oss.str().c_str());
    bool success = PQresultStatus(res) == PGRES_COMMAND_OK;
    if (!success) {
        std::cerr << "Failed to update exercise session end: " << PQerrorMessage(conn) << std::endl;
    }
    PQclear(res);
    return success;
}

std::vector<std::map<std::string, std::string>> DatabaseManager::fetch_exercise_history(int user_id) {
    if (!conn) {
        return {};
    }

    std::ostringstream oss;
    oss << "SELECT eh.id, e.name as exercise_name, eh.session_date, eh.duration_minutes, eh.calories_burned, eh.notes, eh.status, eh.performed_seconds "
        << "FROM exercise_history eh "
        << "JOIN exercises e ON eh.exercise_id = e.id "
        << "WHERE eh.user_id = " << user_id << " "
        << "ORDER BY eh.session_date DESC;";
    
    PGresult *res = PQexec(conn, oss.str().c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Query failed: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        return {};
    }

    std::vector<std::map<std::string, std::string>> history_records;
    int nRows = PQntuples(res);
    int nCols = PQnfields(res);

    for (int i = 0; i < nRows; i++) {
        std::map<std::string, std::string> record;
        for (int j = 0; j < nCols; j++) {
            std::string field_name = PQfname(res, j);
            std::string field_value = PQgetvalue(res, i, j);
            record[field_name] = field_value;
        }
        history_records.push_back(record);
    }

    PQclear(res);
    return history_records;
}

bool DatabaseManager::insert_reference_pose(int exercise_id, int keypoint_index, float x, float y, float confidence, int frame_number) {
    if (!conn) return false;
    std::ostringstream oss;
    oss << "INSERT INTO reference_poses (exercise_id, keypoint_index, x, y, confidence, frame_number) VALUES ("
        << exercise_id << ", " << keypoint_index << ", " << x << ", " << y << ", " << confidence << ", " << frame_number << ");";
    PGresult* res = PQexec(conn, oss.str().c_str());
    bool success = PQresultStatus(res) == PGRES_COMMAND_OK;
    if (!success) {
        std::cerr << "Failed to insert reference pose: " << PQerrorMessage(conn) << std::endl;
    }
    PQclear(res);
    return success;
}

std::vector<std::array<float, 3>> DatabaseManager::fetch_reference_pose(int exercise_id) {
    std::vector<std::array<float, 3>> keypoints(17, {0, 0, 0});
    if (!conn) return keypoints;
    std::ostringstream oss;
    oss << "SELECT keypoint_index, x, y, confidence FROM reference_poses WHERE exercise_id = " << exercise_id << " ORDER BY keypoint_index ASC;";
    PGresult* res = PQexec(conn, oss.str().c_str());
    if (PQresultStatus(res) == PGRES_TUPLES_OK) {
        int nRows = PQntuples(res);
        for (int i = 0; i < nRows; ++i) {
            int idx = std::stoi(PQgetvalue(res, i, 0));
            float x = std::stof(PQgetvalue(res, i, 1));
            float y = std::stof(PQgetvalue(res, i, 2));
            float conf = std::stof(PQgetvalue(res, i, 3));
            if (idx >= 0 && idx < 17) {
                keypoints[idx] = {x, y, conf};
            }
        }
    }
    PQclear(res);
    return keypoints;
}

std::vector<User> DatabaseManager::fetch_users(const std::string& query) {
    if (!conn) {
        return {};
    }

    PGresult *res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Query failed: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        return {};
    }

    std::vector<User> users;
    int nRows = PQntuples(res);

    for (int i = 0; i < nRows; i++) {
        int id = std::stoi(PQgetvalue(res, i, 0));
        Glib::ustring username = PQgetvalue(res, i, 1);
        Glib::ustring email = PQgetvalue(res, i, 2);
        Glib::ustring password_hash = PQgetvalue(res, i, 3);
        Glib::ustring first_name = PQgetvalue(res, i, 4);
        Glib::ustring last_name = PQgetvalue(res, i, 5);
        Glib::ustring created_at = PQgetvalue(res, i, 6);
        Glib::ustring last_login = PQgetvalue(res, i, 7);
        bool is_active = (PQgetvalue(res, i, 8)[0] == 't');

        users.emplace_back(id, username, email, password_hash, first_name, last_name, created_at, last_login, is_active);
    }

    PQclear(res);
    return users;
}

bool DatabaseManager::execute_query(const std::string& query) {
    if (!conn) return false;
    
    PGresult* res = PQexec(conn, query.c_str());
    bool success = PQresultStatus(res) == PGRES_COMMAND_OK;
    if (!success) {
        std::cerr << "Query failed: " << PQerrorMessage(conn) << std::endl;
    }
    PQclear(res);
    return success;
}

std::vector<std::map<std::string, std::string>> DatabaseManager::fetch_sessions(const std::string& query) {
    if (!conn) {
        return {};
    }

    PGresult *res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Query failed: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        return {};
    }

    std::vector<std::map<std::string, std::string>> sessions;
    int nRows = PQntuples(res);
    int nCols = PQnfields(res);

    for (int i = 0; i < nRows; i++) {
        std::map<std::string, std::string> session;
        for (int j = 0; j < nCols; j++) {
            std::string field_name = PQfname(res, j);
            std::string field_value = PQgetvalue(res, i, j);
            session[field_name] = field_value;
        }
        sessions.push_back(session);
    }

    PQclear(res);
    return sessions;
}

std::vector<std::map<std::string, std::string>> DatabaseManager::fetch_statistics(const std::string& query) {
    if (!conn) {
        return {};
    }

    PGresult *res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Query failed: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        return {};
    }

    std::vector<std::map<std::string, std::string>> statistics;
    int nRows = PQntuples(res);
    int nCols = PQnfields(res);

    for (int i = 0; i < nRows; i++) {
        std::map<std::string, std::string> stat;
        for (int j = 0; j < nCols; j++) {
            std::string field_name = PQfname(res, j);
            std::string field_value = PQgetvalue(res, i, j);
            stat[field_name] = field_value;
        }
        statistics.push_back(stat);
    }

    PQclear(res);
    return statistics;
}

bool DatabaseManager::update_user_profile(const User& user) {
    if (!conn) {
        std::cerr << "Database connection not established." << std::endl;
        return false;
    }

    std::ostringstream oss;
    oss << "UPDATE users SET email = '" << user.get_email()
        << "', first_name = '" << user.get_first_name()
        << "', last_name = '" << user.get_last_name()
        << "' WHERE id = " << user.get_id() << ";";

    PGresult* res = PQexec(conn, oss.str().c_str());
    bool success = PQresultStatus(res) == PGRES_COMMAND_OK;
    if (!success) {
        std::cerr << "Failed to update user profile: " << PQerrorMessage(conn) << std::endl;
    }
    PQclear(res);
    return success;
}
