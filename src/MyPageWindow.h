#ifndef MYPAGEWINDOW_H
#define MYPAGEWINDOW_H

#include <gtkmm.h>
#include "User.h"
#include "PasswordChangeWindow.h" // Include the new password change window header
#include "ProfileEditWindow.h"    // Include the new profile edit window header
#include "ExerciseHistoryWindow.h" // Include the new history window header

class MyPageWindow : public Gtk::Window {
public:
    explicit MyPageWindow(Gtk::Window& parent, const User& user); // Add parent and user parameters
    
    void updateUserInfo(const User& user);
    void refreshStatistics();

    // Signal to emit when profile edit is requested
    sigc::signal<void(const User&)>& signal_edit_profile_request();

    // Signal to emit when password change is requested
    sigc::signal<void()>& signal_change_password_request();

private:
    void on_edit_profile_clicked();
    void on_change_password_clicked();
    void on_view_history_clicked();
    void on_back_clicked();
    void on_logout_clicked();

    // Signal
    sigc::signal<void()> m_signal_change_password_request;
    sigc::signal<void(const User&)> m_signal_edit_profile_request; // New signal for profile edit
    
    // UI elements
    Gtk::Box main_box;
    Gtk::Box profile_box;
    Gtk::Box stats_box;
    Gtk::Box actions_box;
    
    // Profile section
    Gtk::Label* profile_title;
    Gtk::Label* username_label;
    Gtk::Label* email_label;
    Gtk::Label* name_label;
    Gtk::Label* join_date_label;
    Gtk::Label* last_login_label;
    
    // Statistics section
    Gtk::Label* stats_title;
    Gtk::Label* total_sessions_label;
    Gtk::Label* total_duration_label;
    Gtk::Label* total_calories_label;
    Gtk::Label* favorite_exercise_label;
    
    // Action buttons
    Gtk::Button* edit_profile_button;
    Gtk::Button* change_password_button;
    Gtk::Button* view_history_button;
    Gtk::Button* back_button;
    Gtk::Button* logout_button;
    
    // Current user (now a reference, initialized in constructor)
    const User& m_user;

    std::unique_ptr<ExerciseHistoryWindow> m_history_window; // Manage history window
};

#endif // MYPAGEWINDOW_H
