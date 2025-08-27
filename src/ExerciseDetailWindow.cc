#include "ExerciseDetailWindow.h"
#include "PoseDetectionWindow.h"
#include "AuthManager.h" // Added for AuthManager::getInstance()
#include <iostream> // For std::cout, std::cerr

extern AuthManager* global_auth_manager; // Declare global_auth_manager

ExerciseDetailWindow::ExerciseDetailWindow(const Exercise& exercise)
    : main_box(Gtk::Orientation::VERTICAL), exercise(exercise), m_exercise_history_id(-1) { // Initialize m_exercise_history_id
    set_title("Exercise Details");
    set_default_size(400, 600);

    // Log exercise session start with "opened" status
    if (global_db_manager && global_auth_manager && global_auth_manager->isLoggedIn()) {
        int user_id = global_auth_manager->getCurrentUser().get_id();
        m_exercise_history_id = global_db_manager->insert_exercise_session_start(user_id, exercise.get_id(), "確認");
        if (m_exercise_history_id != -1) {
            std::cout << "Logged exercise session start (確認) with ID: " << m_exercise_history_id << std::endl;
        } else {
            std::cerr << "Failed to log exercise session start (確認)." << std::endl;
        }
    } else {
        std::cerr << "Cannot log exercise session (確認): User not logged in or DB manager not initialized." << std::endl;
    }

    title_label.set_markup("<big><b>" + exercise.get_name() + "</b></big>");
    tips_label.set_text(exercise.get_name() + "のやり方:");
    try_button.set_label("Try");

    main_box.set_margin(10);
    main_box.set_spacing(10);
    main_box.append(title_label);
    main_box.append(tips_label);
    main_box.append(try_button);

    // Connect the Try button to open the pose detection window
    try_button.signal_clicked().connect([this, &exercise]() {
        // Log a new exercise session with "試行" status when the "Try" button is pressed
        if (global_db_manager && global_auth_manager && global_auth_manager->isLoggedIn()) {
            int user_id = global_auth_manager->getCurrentUser().get_id();
            // Create a new history entry for "試行" status
            m_exercise_history_id = global_db_manager->insert_exercise_session_start(user_id, exercise.get_id(), "試行");
            if (m_exercise_history_id != -1) {
                std::cout << "Logged exercise session start (試行) with ID: " << m_exercise_history_id << std::endl;
            } else {
                std::cerr << "Failed to log exercise session start (試行)." << std::endl;
            }
        } else {
            std::cerr << "Cannot log exercise session (試行): User not logged in or DB manager not initialized." << std::endl;
        }

        // Pass the exercise and the new history ID to the PoseDetectionWindow
        auto pose_window = Gtk::make_managed<PoseDetectionWindow>(exercise, m_exercise_history_id);
        pose_window->set_transient_for(*this);  // Make it a transient window
        pose_window->show();
    });

    set_child(main_box);
    show();
}
