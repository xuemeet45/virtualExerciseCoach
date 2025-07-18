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
        Glib::ustring calories_burned_estimate = PQgetvalue(res, i, 15);
        Glib::ustring updated_at = PQgetvalue(res, i, 16);

        exercises.emplace_back(id, name, image_path, category, primary_muscle, secondary_muscles,
                               equipment, difficulty_level, instructions, tips, video_url,
                               reps_sets_suggestion, benefits, common_mistakes, variations,
                               calories_burned_estimate, updated_at);
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
