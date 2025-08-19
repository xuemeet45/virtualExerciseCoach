#ifndef PROFILE_EDIT_WINDOW_H
#define PROFILE_EDIT_WINDOW_H

#include <gtkmm.h>
#include <string>
#include "User.h"

class ProfileEditWindow : public Gtk::Window {
public:
    explicit ProfileEditWindow(Gtk::Window& parent, const User& user);
    ~ProfileEditWindow() override;

    // Signal to emit when profile update is successful
    sigc::signal<void(const User&)>& signal_profile_update_success();
    // Signal to emit when the user wants to go back to MyPage
    sigc::signal<void()>& signal_back_to_mypage();

protected:
    // Signal handlers
    void on_save_button_clicked();
    void on_cancel_button_clicked();

    // Child widgets
    Gtk::Grid m_grid;
    Gtk::Label m_label_username;
    Gtk::Entry m_entry_username;
    Gtk::Label m_label_email;
    Gtk::Entry m_entry_email;
    Gtk::Label m_label_first_name;
    Gtk::Entry m_entry_first_name;
    Gtk::Label m_label_last_name;
    Gtk::Entry m_entry_last_name;
    Gtk::Button m_button_save;
    Gtk::Button m_button_cancel;

    // Signals
    sigc::signal<void(const User&)> m_signal_profile_update_success;
    sigc::signal<void()> m_signal_back_to_mypage;

    Gtk::Window& m_parent_window;
    User m_current_user;
};

#endif // PROFILE_EDIT_WINDOW_H
