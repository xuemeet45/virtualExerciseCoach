#ifndef VIRTUALFITNESSCOACHWINDOW_H
#define VIRTUALFITNESSCOACHWINDOW_H

#include <gtkmm.h>
#include <memory>
#include <vector>
#include "DatabaseManager.h"
#include "PoseDetectionWindow.h"

class VirtualFitnessCoachWindow : public Gtk::ApplicationWindow {
public:
    VirtualFitnessCoachWindow();

private:
    void add_exercise_cards(const std::vector<Exercise>& exercises);
    void show_exercise_detail(const Exercise& exercise);

    Gtk::Box main_box, sidebar, content_area;
    Gtk::Stack content_stack; 
    Gtk::Label title_label;
    Gtk::Grid exercise_grid;
    Gtk::Box exercise_list_page;
    Gtk::ScrolledWindow scrolled_window;
    Gtk::Button back_button;
    std::vector<std::unique_ptr<PoseDetectionWindow>> pose_windows;
};

#endif // VIRTUALFITNESSCOACHWINDOW_H
