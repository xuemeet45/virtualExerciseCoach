#ifndef REGISTERWINDOW_H
#define REGISTERWINDOW_H

#include <gtkmm.h>

class RegisterWindow : public Gtk::Window {
public:
    RegisterWindow();
    
    // Signal for successful registration
    sigc::signal<void()> signal_register_success;
    sigc::signal<void()> signal_back_to_login;

private:
    void on_register_clicked();
    void on_back_clicked();
    bool validate_form();
    
    // UI elements
    Gtk::Box main_box;
    Gtk::Grid form_grid;
    Gtk::Label title_label;
    Gtk::Label username_label;
    Gtk::Entry username_entry;
    Gtk::Label email_label;
    Gtk::Entry email_entry;
    Gtk::Label password_label;
    Gtk::Entry password_entry;
    Gtk::Label confirm_password_label;
    Gtk::Entry confirm_password_entry;
    Gtk::Label first_name_label;
    Gtk::Entry first_name_entry;
    Gtk::Label last_name_label;
    Gtk::Entry last_name_entry;
    Gtk::Button register_button;
    Gtk::Button back_button;
    Gtk::Label status_label;
};

#endif // REGISTERWINDOW_H
