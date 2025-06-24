#include "VirtualFitnessCoachWindow.h"
#include "ExerciseDetailWindow.h" // Include the header for ExerciseDetailWindow
#include "PoseDetectionWindow.h" // Include the header for PoseDetectionWindow

VirtualFitnessCoachWindow::VirtualFitnessCoachWindow()
    : main_box(Gtk::Orientation::HORIZONTAL),
      sidebar(Gtk::Orientation::VERTICAL),
      content_stack(), // Use Gtk::Stack to manage different views
      exercise_list_page(),
      exercise_grid(),
      back_button("戻る") {
    set_title("バーチャルエクササイズコーチ");
    set_default_size(800, 600);

    // Sidebar
    auto profile_button = Gtk::make_managed<Gtk::Button>("マイページ");
    auto exercise_button = Gtk::make_managed<Gtk::Button>("エクササイズライブラリ");
    auto history_button = Gtk::make_managed<Gtk::Button>("履歴");
    auto logout_button = Gtk::make_managed<Gtk::Button>("ログアウト");

    sidebar.set_margin(10);
    sidebar.set_spacing(10);
    sidebar.append(*profile_button);
    sidebar.append(*exercise_button);
    sidebar.append(*history_button);
    sidebar.append(*logout_button);
    sidebar.set_valign(Gtk::Align::FILL);
    sidebar.set_hexpand(false);
    sidebar.set_size_request(200);

    // Exercise List Page
    title_label.set_markup("<big><b>エクササイズライブラリ</b></big>");
    title_label.set_halign(Gtk::Align::CENTER);
    title_label.set_margin_bottom(16);

    auto exercise_list_vbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 10);
    exercise_list_vbox->set_margin(10);
    exercise_list_vbox->append(title_label);

    scrolled_window.set_child(exercise_grid);
    scrolled_window.set_expand();
    exercise_list_vbox->append(scrolled_window);

    exercise_list_page.append(*exercise_list_vbox);

    content_stack.add(exercise_list_page, "exercise_list");
    content_stack.set_visible_child("exercise_list");

    // Fetch exercises from the database and add rows
    std::string query = "SELECT id, name, image_path, category, primary_muscle, secondary_muscles, equipment, difficulty_level, instructions, tips, video_url, reps_sets_suggestion, benefits, common_mistakes, variations, calories_burned_estimate, updated_at FROM public.exercises";
    auto exercises = global_db_manager->fetch_exercises(query);
    add_exercise_cards(exercises);

    // Combine UI
    main_box.append(sidebar);
    main_box.append(content_stack);
    set_child(main_box);

    // Connect signals for sidebar buttons (you'll need to implement these)
    exercise_button->signal_clicked().connect([this]() {
        content_stack.set_visible_child("exercise_list");
    });
}

void VirtualFitnessCoachWindow::add_exercise_cards(const std::vector<Exercise>& exercises) {
    int col = 0;
    int row_index = 0;
    for (const auto& exercise : exercises) {
        auto create_label = [](const std::string& text, const std::string& tooltip) {
            auto label = Gtk::make_managed<Gtk::Label>();
            auto utf8_substr = [](const std::string& str, size_t max_len) {
                Glib::ustring ustr = str;
                return ustr.bytes() > max_len ? ustr.substr(0, max_len) + "..." : ustr;
            };
            label->set_text(utf8_substr(text, 6));
            label->set_wrap(true);
            label->set_expand(false);
            label->set_tooltip_text(tooltip);
            return label;
        };

        auto create_button = [](const std::string& text) {
            auto button = Gtk::make_managed<Gtk::Button>(text);
            button->set_expand(false);
            return button;
        };

        auto card_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 5);
        card_box->set_margin(10);
        card_box->set_size_request(80, 80);
        card_box->set_hexpand(false);

        // Load image from file path
        auto image = Gtk::make_managed<Gtk::Image>(exercise.get_image_path());
        image->set_pixel_size(64);
        card_box->append(*image);

        // Add label and button
        card_box->append(*create_label(exercise.get_name(), exercise.get_name()));

        auto view_button = create_button("詳細を見る");
        view_button->signal_clicked().connect([this, exercise]() {
            show_exercise_detail(exercise);
        });
        card_box->append(*view_button);

        exercise_grid.attach(*card_box, col, row_index);

        // Move to the next column or row
        if (++row_index == 4) {
            row_index = 0;
            ++col;
        }
    }
    exercise_grid.set_visible(true); // Make sure the grid and its contents are visible
}

void VirtualFitnessCoachWindow::show_exercise_detail(const Exercise& exercise) {
    // Create the detail view
    auto detail_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 10);
    detail_box->set_margin(10);

    auto title_label = Gtk::make_managed<Gtk::Label>();
    title_label->set_markup("<big><b>" + exercise.get_name() + "</b></big>");
    detail_box->append(*title_label);

    auto image = Gtk::make_managed<Gtk::Image>(exercise.get_image_path());
    image->set_pixel_size(128);
    detail_box->append(*image);
    image->set_halign(Gtk::Align::CENTER);
    image->set_valign(Gtk::Align::CENTER);

    auto category_label = Gtk::make_managed<Gtk::Label>("<b>カテゴリー:</b> " + exercise.get_category());
    category_label->set_use_markup(true);
    detail_box->append(*category_label);

    auto primary_muscle_label = Gtk::make_managed<Gtk::Label>("<b>主要筋肉:</b> " + exercise.get_primary_muscle());
    primary_muscle_label->set_use_markup(true);
    detail_box->append(*primary_muscle_label);

    auto instructions_label = Gtk::make_managed<Gtk::Label>("<b>やり方:</b>");
    instructions_label->set_use_markup(true);
    detail_box->append(*instructions_label);
    auto instructions_text_view = Gtk::make_managed<Gtk::TextView>();
    auto instructions_buffer = instructions_text_view->get_buffer();
    instructions_buffer->set_text(exercise.get_instructions());
    instructions_text_view->set_editable(false);
    instructions_text_view->set_cursor_visible(false);
    auto instructions_scroll = Gtk::make_managed<Gtk::ScrolledWindow>();
    instructions_scroll->set_child(*instructions_text_view);
    instructions_scroll->set_policy(Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC);
    detail_box->append(*instructions_scroll);
    instructions_scroll->set_expand(true);
    instructions_scroll->set_halign(Gtk::Align::FILL);
    instructions_scroll->set_valign(Gtk::Align::FILL);

    // Create and add the Try button
    auto try_button = Gtk::make_managed<Gtk::Button>("試す");
    try_button->signal_clicked().connect([this, exercise]() {
        auto pose_window = std::make_unique<PoseDetectionWindow>(exercise);
        pose_window->set_transient_for(*this);
        pose_window->set_modal(true);
        pose_window->show();
        pose_windows.push_back(std::move(pose_window));  // Store the window
    });
    detail_box->append(*try_button);

    back_button.signal_clicked().connect([this]() {
        // Switch back to the exercise list view
        if (auto visible_child = content_stack.get_visible_child()) {
            if (visible_child != &exercise_list_page) {
                content_stack.remove(*visible_child); // Remove only the detail view
            }
        }
        content_stack.set_visible_child("exercise_list");
    });
    back_button.set_margin_top(10);
    back_button.set_halign(Gtk::Align::END);
    back_button.set_valign(Gtk::Align::END);
    detail_box->append(back_button);

    // Add the detail view to the stack
    content_stack.add(*detail_box, "exercise_detail_" + std::to_string(exercise.get_id()));
    content_stack.set_visible_child("exercise_detail_" + std::to_string(exercise.get_id()));
}