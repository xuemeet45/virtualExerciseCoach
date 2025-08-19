#ifndef PASSWORD_CHANGE_WINDOW_H
#define PASSWORD_CHANGE_WINDOW_H

#include <gtkmm.h>
#include <string>

class PasswordChangeWindow : public Gtk::Window {
public:
    explicit PasswordChangeWindow(Gtk::Window& parent); // Add parent parameter
    ~PasswordChangeWindow() override;

    // Signal to emit when password change is successful
    sigc::signal<void()>& signal_password_change_success();
    // Signal to emit when the user wants to go back to MyPage
    sigc::signal<void()>& signal_back_to_mypage();

protected:
    // Signal handlers
    void on_change_password_button_clicked();
    void on_cancel_button_clicked();

    // Child widgets
    Gtk::Grid m_grid;
    Gtk::Label m_label_old_password;
    Gtk::Entry m_entry_old_password;
    Gtk::Label m_label_new_password;
    Gtk::Entry m_entry_new_password;
    Gtk::Label m_label_confirm_new_password;
    Gtk::Entry m_entry_confirm_new_password;
    Gtk::CheckButton m_checkbutton_show_password; // New check button
    Gtk::Button m_button_change_password;
    Gtk::Button m_button_cancel;

    // Signals
    sigc::signal<void()> m_signal_password_change_success;
    sigc::signal<void()> m_signal_back_to_mypage;

    Gtk::Window& m_parent_window; // Store parent window reference
};

#endif // PASSWORD_CHANGE_WINDOW_H
