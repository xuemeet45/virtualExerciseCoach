#ifndef EXERCISEDETAILWINDOW_H
#define EXERCISEDETAILWINDOW_H

#include <gtkmm.h>
#include "DatabaseManager.h"

class ExerciseDetailWindow : public Gtk::Window {
public:
    ExerciseDetailWindow(const Exercise& exercise);

private:
    void on_try_button_clicked();

    Gtk::Box main_box;
    Gtk::Box image_box;
    Gtk::Box details_box;
    Gtk::Label title_label;
    Gtk::Image image;
    Gtk::Label category_label;
    Gtk::Label primary_muscle_label;
    Gtk::Label secondary_muscles_label;
    Gtk::Label equipment_label;
    Gtk::Label difficulty_label;
    Gtk::Label instructions_label;
    Gtk::TextView instructions_text;
    Gtk::ScrolledWindow instructions_scroll;
    Gtk::Label tips_label;
    Gtk::TextView tips_text;
    Gtk::ScrolledWindow tips_scroll;
    Gtk::Label video_url_label;
    Gtk::Label reps_sets_label;
    Gtk::Label benefits_label;
    Gtk::Label common_mistakes_label;
    Gtk::Label variations_label;
    Gtk::Label calories_label;
    Gtk::Label updated_at_label;
    Gtk::Button try_button;
    Exercise exercise;
};

#endif // EXERCISEDETAILWINDOW_H
