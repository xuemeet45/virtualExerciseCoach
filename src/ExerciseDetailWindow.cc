#include "ExerciseDetailWindow.h"
#include "PoseDetectionWindow.h"

ExerciseDetailWindow::ExerciseDetailWindow(const Exercise& exercise)
    : main_box(Gtk::Orientation::VERTICAL), exercise(exercise) {
    set_title("Exercise Details");
    set_default_size(400, 600);

    title_label.set_markup("<big><b>" + exercise.get_name() + "</b></big>");
    tips_label.set_text(exercise.get_name() + "のやり方:");
    try_button.set_label("Try");

    main_box.set_margin(10);
    main_box.set_spacing(10);
    main_box.append(title_label);
    main_box.append(tips_label);
    main_box.append(try_button);

    // Connect the Try button to open the pose detection window
    try_button.signal_clicked().connect([this, &exercise]() {
        auto pose_window = Gtk::make_managed<PoseDetectionWindow>(exercise);
        pose_window->set_transient_for(*this);  // Make it a transient window
        pose_window->show();
    });

    set_child(main_box);
    show();
}

