#ifndef EXERCISE_HISTORY_WINDOW_H
#define EXERCISE_HISTORY_WINDOW_H

#include <gtkmm.h>
#include <vector>
#include <map>
#include <string>
#include "DatabaseManager.h" // For fetching history
#include "AuthManager.h"     // For current user ID

class ExerciseHistoryWindow : public Gtk::Window {
public:
    explicit ExerciseHistoryWindow(Gtk::Window& parent);
    ~ExerciseHistoryWindow() override;

protected:
    void on_show() override; // Refresh data when shown
    void on_back_button_clicked();

private:
    void load_history_data();

    Gtk::Box m_main_box;
    Gtk::Label m_title_label;
    Gtk::ScrolledWindow m_scrolled_window;
    Gtk::ListBox m_history_list_box; // Using Gtk::ListBox for simpler custom display
    Gtk::Button m_back_button;
};

#endif // EXERCISE_HISTORY_WINDOW_H
