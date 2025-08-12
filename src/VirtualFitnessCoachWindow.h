#ifndef VIRTUALFITNESSCOACHWINDOW_H
#define VIRTUALFITNESSCOACHWINDOW_H

#include <gtkmm.h>
#include <memory>
#include <vector>
#include "DatabaseManager.h"
#include "PoseDetectionWindow.h"
#include "MyPageWindow.h"
#include "AuthManager.h"

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

    Gtk::Box main_box, sidebar, content_area;
    Gtk::Stack content_stack; 
    Gtk::Label title_label;
    Gtk::Grid exercise_grid;
    Gtk::Box exercise_list_page;
    Gtk::ScrolledWindow scrolled_window;
    Gtk::Button back_button;
    std::vector<std::unique_ptr<PoseDetectionWindow>> pose_windows;
    std::unique_ptr<MyPageWindow> my_page_window;
};

#endif // VIRTUALFITNESSCOACHWINDOW_H
