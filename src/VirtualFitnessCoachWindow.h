#ifndef VIRTUALFITNESSCOACHWINDOW_H
#define VIRTUALFITNESSCOACHWINDOW_H

#include <gtkmm.h>
#include <memory>
#include <vector>
#include "DatabaseManager.h"
// #include "PoseDetectionWindow.h" // PoseDetectionWindow will be opened by ExerciseDetailWindow
#include "MyPageWindow.h"
#include "AuthManager.h"
#include "PasswordChangeWindow.h"
#include "ProfileEditWindow.h"
#include "ExerciseDetailWindow.h" // Include ExerciseDetailWindow
#include "ExerciseHistoryWindow.h" // Include ExerciseHistoryWindow

class VirtualFitnessCoachWindow : public Gtk::ApplicationWindow {
public:
    VirtualFitnessCoachWindow();

    // Signal for logout request
    sigc::signal<void()> signal_logout_request;

private:
    void add_exercise_cards(const std::vector<Exercise>& exercises);
    void show_exercise_detail(const Exercise& exercise);
    void on_my_page_clicked();
    void on_logout_clicked();
    void on_history_clicked(); // New method to show exercise history window
    void show_password_change_window(); // New method to show password change window
    void show_profile_edit_window(const User& user); // New method to show profile edit window
    void on_profile_update_success(const User& updated_user); // New method to handle profile update success

    Gtk::Box main_box, sidebar, content_area;
    Gtk::Stack content_stack; 
    Gtk::Label title_label;
    Gtk::Grid exercise_grid;
    Gtk::Box exercise_list_page;
    Gtk::ScrolledWindow scrolled_window;
    Gtk::Button back_button;
    // std::vector<std::unique_ptr<PoseDetectionWindow>> pose_windows; // Removed
    std::unique_ptr<MyPageWindow> my_page_window; // Changed to unique_ptr for explicit ownership
    std::unique_ptr<PasswordChangeWindow> password_change_window; // Changed to unique_ptr
    std::unique_ptr<ProfileEditWindow> profile_edit_window; // Changed to unique_ptr
    std::unique_ptr<ExerciseHistoryWindow> exercise_history_window; // Changed to unique_ptr
    std::unique_ptr<ExerciseDetailWindow> m_exercise_detail_window; // New unique pointer for ExerciseDetailWindow
};

#endif // VIRTUALFITNESSCOACHWINDOW_H
