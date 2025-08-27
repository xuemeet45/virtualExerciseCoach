#include "VirtualFitnessCoachWindow.h"
#include "ExerciseDetailWindow.h" // Include the header for ExerciseDetailWindow
#include "ExerciseHistoryWindow.h" // Include the header for ExerciseHistoryWindow
#include <iostream> // Required for std::cout and std::endl
// #include "PoseDetectionWindow.h" // PoseDetectionWindow will be opened by ExerciseDetailWindow

VirtualFitnessCoachWindow::VirtualFitnessCoachWindow()
    : main_box(Gtk::Orientation::HORIZONTAL),
      sidebar(Gtk::Orientation::VERTICAL),
      content_stack(), // Use Gtk::Stack to manage different views
      exercise_list_page(),
      exercise_grid(),
      back_button("戻る"),
      my_page_window(nullptr), // Initialize raw pointer
      password_change_window(nullptr), // Initialize raw pointer
      profile_edit_window(nullptr), // Initialize raw pointer
      exercise_history_window(nullptr), // Initialize raw pointer
      m_exercise_detail_window(nullptr) {
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

    content_stack.add(exercise_list_page, "exercise_list"); // Add the actual exercise list page
    content_stack.set_visible_child("exercise_list");

    // Fetch exercises from the database and add rows
    std::string query = "SELECT id, name, image_path, category, primary_muscle, secondary_muscles, equipment, difficulty_level, instructions, tips, video_url, reps_sets_suggestion, benefits, common_mistakes, variations, calories_burned_estimate, updated_at FROM public.exercises";
    auto exercises = global_db_manager->fetch_exercises(query);
    add_exercise_cards(exercises);

    // Combine UI
    main_box.append(sidebar);
    main_box.append(content_stack);
    set_child(main_box);

    // Connect signals for sidebar buttons
    profile_button->signal_clicked().connect(sigc::mem_fun(*this, &VirtualFitnessCoachWindow::on_my_page_clicked));
    exercise_button->signal_clicked().connect([this]() {
        content_stack.set_visible_child("exercise_list");
    });
    history_button->signal_clicked().connect(sigc::mem_fun(*this, &VirtualFitnessCoachWindow::on_history_clicked));
    logout_button->signal_clicked().connect(sigc::mem_fun(*this, &VirtualFitnessCoachWindow::on_logout_clicked));
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
    // Create and show the ExerciseDetailWindow
    m_exercise_detail_window = std::make_unique<ExerciseDetailWindow>(exercise);
    m_exercise_detail_window->set_transient_for(*this);
    m_exercise_detail_window->set_modal(true);
    m_exercise_detail_window->show();

    // Connect a signal to handle when ExerciseDetailWindow is closed
    m_exercise_detail_window->signal_hide().connect([this]() {
        if (m_exercise_detail_window) {
            m_exercise_detail_window->hide();
            // Optionally, reset the unique_ptr if the window should be recreated each time
            // m_exercise_detail_window.reset(); 
        }
        // Ensure the main window is focused or visible after the modal closes
        present(); 
    });
}

void VirtualFitnessCoachWindow::on_my_page_clicked() {
    if (!my_page_window) {
        my_page_window = Gtk::make_managed<MyPageWindow>(*this); // Create and manage with GTK
        // Connect the signal from MyPageWindow to show the password change window
        my_page_window->signal_change_password_request().connect(
            sigc::mem_fun(*this, &VirtualFitnessCoachWindow::show_password_change_window));
        // Connect the signal from MyPageWindow to show the profile edit window
        my_page_window->signal_edit_profile_request().connect(
            sigc::mem_fun(*this, &VirtualFitnessCoachWindow::show_profile_edit_window));
        
        // Connect to the hide signal to nullify the pointer when the window is closed
        my_page_window->signal_hide().connect([this]() {
            std::cout << "MyPageWindow hidden, nullifying pointer." << std::endl;
            my_page_window = nullptr;
        });
    }
    
    if (AuthManager::getInstance().isLoggedIn()) {
        my_page_window->updateUserInfo(AuthManager::getInstance().getCurrentUser());
    }
    
    my_page_window->present();
}

void VirtualFitnessCoachWindow::show_profile_edit_window(const User& user) {
    if (!profile_edit_window) {
        profile_edit_window = Gtk::make_managed<ProfileEditWindow>(*this, user); // Create and manage with GTK
        profile_edit_window->set_modal(true);

        // Connect signals from ProfileEditWindow
        profile_edit_window->signal_profile_update_success().connect(
            sigc::mem_fun(*this, &VirtualFitnessCoachWindow::on_profile_update_success));

        profile_edit_window->signal_back_to_mypage().connect([this]() {
            // User cancelled, hide profile edit window and show MyPage
            if (profile_edit_window) profile_edit_window->hide();
            if (my_page_window) my_page_window->present(); // Show MyPage again
        });
    } else {
        // If window already exists, update its user info and show
        // profile_edit_window->updateUserInfo(user); // Update user info if window already exists
        // (Removed because ProfileEditWindow has no updateUserInfo method)
    }
    if (profile_edit_window) profile_edit_window->present();
}

void VirtualFitnessCoachWindow::on_profile_update_success(const User& updated_user) {
    // Profile updated, hide profile edit window and refresh MyPage
    if (profile_edit_window) {
        profile_edit_window->hide();
    }
    if (my_page_window) {
        my_page_window->updateUserInfo(updated_user);
        my_page_window->present(); // Show MyPage again
    }
}

void VirtualFitnessCoachWindow::show_password_change_window() {
    if (!password_change_window) {
        password_change_window = Gtk::make_managed<PasswordChangeWindow>(*this); // Create and manage with GTK
        password_change_window->set_modal(true);

        // Connect signals from PasswordChangeWindow
        password_change_window->signal_password_change_success().connect([this]() {
            // Password changed, hide password change window and refresh MyPage
            if (password_change_window) password_change_window->hide();
            if (my_page_window) {
                my_page_window->updateUserInfo(AuthManager::getInstance().getCurrentUser());
                my_page_window->present(); // Show MyPage again
            }
        });

        password_change_window->signal_back_to_mypage().connect([this]() {
            // User cancelled, hide password change window and show MyPage
            if (password_change_window) password_change_window->hide();
            if (my_page_window) {
                my_page_window->present(); // Show MyPage again
            }
        });
    }
    if (password_change_window) password_change_window->present();
}

void VirtualFitnessCoachWindow::on_history_clicked() {
    if (!exercise_history_window) {
        exercise_history_window = Gtk::make_managed<ExerciseHistoryWindow>(*this);
        exercise_history_window->set_modal(true);
        exercise_history_window->signal_hide().connect([this]() {
            exercise_history_window = nullptr;
        });
    }
    exercise_history_window->present();
}

void VirtualFitnessCoachWindow::on_logout_clicked() {
    AuthManager::getInstance().logout();
    hide();
    signal_logout_request.emit(); // Emit the logout signal
}
