#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <gtkmm.h>
#include <functional>

class LoginWindow : public Gtk::Window {
public:
    LoginWindow();
    
    // Signal for successful login
    sigc::signal<void()> signal_login_success;
    sigc::signal<void()> signal_register_request;

private:
    void on_login_clicked();
    void on_register_clicked();
    void on_guest_clicked();
    
    // UI elements
    Gtk::Box main_box;
    Gtk::Box form_box;
    Gtk::Label title_label;
    Gtk::Label username_label;
    Gtk::Entry username_entry;
    Gtk::Label password_label;
    Gtk::Entry password_entry;
    Gtk::Button login_button;
    Gtk::Button register_button;
    Gtk::Button guest_button;
    Gtk::Label status_label;
};

#endif // LOGINWINDOW_H 