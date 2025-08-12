#ifndef MYPAGEWINDOW_H
#define MYPAGEWINDOW_H

#include <gtkmm.h>
#include "User.h"

class MyPageWindow : public Gtk::Window {
public:
    MyPageWindow();
    
    void updateUserInfo(const User& user);
    void refreshStatistics();

private:
    void on_edit_profile_clicked();
    void on_change_password_clicked();
    void on_view_history_clicked();
    void on_back_clicked();
    void on_logout_clicked();
    
    // UI elements
    Gtk::Box main_box;
    Gtk::Box profile_box;
    Gtk::Box stats_box;
    Gtk::Box actions_box;
    
    // Profile section
    Gtk::Label profile_title;
    Gtk::Label username_label;
    Gtk::Label email_label;
    Gtk::Label name_label;
    Gtk::Label join_date_label;
    Gtk::Label last_login_label;
    
    // Statistics section
    Gtk::Label stats_title;
    Gtk::Label total_sessions_label;
    Gtk::Label total_duration_label;
    Gtk::Label total_calories_label;
    Gtk::Label favorite_exercise_label;
    
    // Action buttons
    Gtk::Button edit_profile_button;
    Gtk::Button change_password_button;
    Gtk::Button view_history_button;
    Gtk::Button back_button;
    Gtk::Button logout_button;
    
    // Current user
    User current_user;
};

#endif // MYPAGEWINDOW_H 