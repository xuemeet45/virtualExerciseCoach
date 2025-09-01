#include "MyPageWindow.h"
#include "AuthManager.h"
#include "DatabaseManager.h"
#include "ExerciseHistoryWindow.h" // Include the new history window header
#include <iostream>

MyPageWindow::MyPageWindow(Gtk::Window& parent, const User& user) // Accept parent and user parameters
    : Gtk::Window(), // Call base class constructor
      main_box(Gtk::Orientation::VERTICAL),
      profile_box(Gtk::Orientation::VERTICAL),
      stats_box(Gtk::Orientation::VERTICAL),
      actions_box(Gtk::Orientation::VERTICAL),
      profile_title(Gtk::make_managed<Gtk::Label>("プロフィール")), // Initialize Gtk::Label members with make_managed
      username_label(Gtk::make_managed<Gtk::Label>()),
      email_label(Gtk::make_managed<Gtk::Label>()),
      name_label(Gtk::make_managed<Gtk::Label>()),
      join_date_label(Gtk::make_managed<Gtk::Label>()),
      last_login_label(Gtk::make_managed<Gtk::Label>()),
      stats_title(Gtk::make_managed<Gtk::Label>("統計")),
      total_sessions_label(Gtk::make_managed<Gtk::Label>()),
      total_duration_label(Gtk::make_managed<Gtk::Label>()),
      total_calories_label(Gtk::make_managed<Gtk::Label>()),
      favorite_exercise_label(Gtk::make_managed<Gtk::Label>()),
      edit_profile_button(Gtk::make_managed<Gtk::Button>("ユーザー情報編集")),
      change_password_button(Gtk::make_managed<Gtk::Button>("パスワード変更")),
      view_history_button(Gtk::make_managed<Gtk::Button>("運動履歴")),
      back_button(Gtk::make_managed<Gtk::Button>("戻る")),
      logout_button(Gtk::make_managed<Gtk::Button>("ログアウト")),
      m_user(user), // Initialize the user reference
      m_history_window(nullptr) { // Initialize new member
    
    set_title("バーチャルエクササイズコーチ - マイページ");
    set_default_size(600, 700); // Slightly taller for better spacing
    set_modal(true);
    set_deletable(true);
    set_resizable(true);
    set_transient_for(parent); // Set MyPageWindow as transient for the main window
    
    // Apply general window styling
    add_css_class("window");

    // Main layout box
    main_box.set_margin(20);
    main_box.set_spacing(20);
    main_box.set_halign(Gtk::Align::CENTER);
    main_box.set_valign(Gtk::Align::CENTER);

    // Profile section
    Gtk::Box profile_frame_box(Gtk::Orientation::VERTICAL);
    profile_frame_box.add_css_class("mypage-box-frame");
    profile_frame_box.set_spacing(5);

    profile_title->add_css_class("mypage-section-title");
    profile_title->set_halign(Gtk::Align::CENTER);
    profile_frame_box.append(*profile_title);

    Gtk::Grid profile_grid;
    profile_grid.set_row_spacing(5);
    profile_grid.set_column_spacing(10);
    profile_grid.set_margin(10);
    profile_grid.set_halign(Gtk::Align::CENTER);

    auto add_profile_row = [&](const Glib::ustring& label_text, Gtk::Label* value_label, int row) {
        auto label = Gtk::make_managed<Gtk::Label>(label_text);
        label->set_halign(Gtk::Align::START);
        label->add_css_class("mypage-label");
        value_label->set_halign(Gtk::Align::START);
        value_label->add_css_class("mypage-label");
        profile_grid.attach(*label, 0, row, 1, 1);
        profile_grid.attach(*value_label, 1, row, 1, 1);
    };

    add_profile_row("ユーザー名:", username_label, 0);
    add_profile_row("メールアドレス:", email_label, 1);
    add_profile_row("名前:", name_label, 2);
    add_profile_row("登録日:", join_date_label, 3);
    add_profile_row("最終ログイン:", last_login_label, 4);
    
    profile_frame_box.append(profile_grid);
    main_box.append(profile_frame_box);

    // Statistics section
    Gtk::Box stats_frame_box(Gtk::Orientation::VERTICAL);
    stats_frame_box.add_css_class("mypage-box-frame");
    stats_frame_box.set_spacing(5);

    stats_title->add_css_class("mypage-section-title");
    stats_title->set_halign(Gtk::Align::CENTER);
    stats_frame_box.append(*stats_title);

    Gtk::Grid stats_grid;
    stats_grid.set_row_spacing(5);
    stats_grid.set_column_spacing(10);
    stats_grid.set_margin(10);
    stats_grid.set_halign(Gtk::Align::CENTER);

    auto add_stats_row = [&](const Glib::ustring& label_text, Gtk::Label* value_label, int row) {
        auto label = Gtk::make_managed<Gtk::Label>(label_text);
        label->set_halign(Gtk::Align::START);
        label->add_css_class("mypage-label");
        value_label->set_halign(Gtk::Align::START);
        value_label->add_css_class("mypage-label");
        stats_grid.attach(*label, 0, row, 1, 1);
        stats_grid.attach(*value_label, 1, row, 1, 1);
    };

    total_sessions_label->set_text("0"); // Only value
    total_duration_label->set_text("0分");
    total_calories_label->set_text("0kcal");
    favorite_exercise_label->set_text("なし");

    add_stats_row("総セッション数:", total_sessions_label, 0);
    add_stats_row("総運動時間:", total_duration_label, 1);
    add_stats_row("総消費カロリー:", total_calories_label, 2);
    add_stats_row("お気に入りエクササイズ:", favorite_exercise_label, 3);

    stats_frame_box.append(stats_grid);
    main_box.append(stats_frame_box);
    
    // Action buttons
    Gtk::Box actions_frame_box(Gtk::Orientation::VERTICAL);
    actions_frame_box.add_css_class("mypage-box-frame");
    actions_frame_box.set_spacing(10);
    actions_frame_box.set_halign(Gtk::Align::CENTER);

    edit_profile_button->add_css_class("mypage-button");
    edit_profile_button->set_halign(Gtk::Align::CENTER);
    edit_profile_button->set_size_request(200, -1); // Fixed width for buttons

    change_password_button->add_css_class("mypage-button");
    change_password_button->set_halign(Gtk::Align::CENTER);
    change_password_button->set_size_request(200, -1);

    view_history_button->add_css_class("mypage-button");
    view_history_button->set_halign(Gtk::Align::CENTER);
    view_history_button->set_size_request(200, -1);

    back_button->add_css_class("mypage-button");
    back_button->set_halign(Gtk::Align::CENTER);
    back_button->set_size_request(200, -1);

    logout_button->add_css_class("mypage-button");
    logout_button->add_css_class("destructive"); // Apply destructive style
    logout_button->set_halign(Gtk::Align::CENTER);
    logout_button->set_size_request(200, -1);
    
    actions_frame_box.append(*edit_profile_button);
    actions_frame_box.append(*change_password_button);
    actions_frame_box.append(*view_history_button);
    actions_frame_box.append(*back_button);
    actions_frame_box.append(*logout_button);
    
    main_box.append(actions_frame_box);
    
    set_child(main_box);
    
    // Connect signals
    edit_profile_button->signal_clicked().connect(sigc::mem_fun(*this, &MyPageWindow::on_edit_profile_clicked));
    change_password_button->signal_clicked().connect(sigc::mem_fun(*this, &MyPageWindow::on_change_password_clicked));
    view_history_button->signal_clicked().connect(sigc::mem_fun(*this, &MyPageWindow::on_view_history_clicked));
    back_button->signal_clicked().connect(sigc::mem_fun(*this, &MyPageWindow::on_back_clicked));
    logout_button->signal_clicked().connect(sigc::mem_fun(*this, &MyPageWindow::on_logout_clicked));
    
    // Update UI with initial user info
    updateUserInfo(m_user);

    set_visible(true);
}

void MyPageWindow::updateUserInfo(const User& user) {
    // m_user = user; // m_user is a const reference, cannot be reassigned
    
    username_label->set_text(user.get_username());
    email_label->set_text(user.get_email());
    name_label->set_text(user.get_last_name() + " " + user.get_first_name());
    join_date_label->set_text(user.get_created_at());
    last_login_label->set_text(user.get_last_login());
    
    refreshStatistics();
}

void MyPageWindow::refreshStatistics() {
    if (!global_db_manager || m_user.get_id() == 0) {
        return;
    }
    
    // Get exercise history statistics
    std::ostringstream oss;
    oss << "SELECT COUNT(*) as total_sessions, "
        << "COALESCE(SUM(duration_minutes), 0) as total_duration, "
        << "COALESCE(SUM(calories_burned), 0) as total_calories "
        << "FROM exercise_history WHERE user_id = " << m_user.get_id();
    
    auto stats = global_db_manager->fetch_statistics(oss.str());
    if (!stats.empty()) {
        const auto& stat = stats[0];
        
        // Helper to safely get integer from map
        auto get_int_or_default = [](const std::map<std::string, std::string>& m, const std::string& key, int default_val = 0) {
            if (m.count(key) && !m.at(key).empty()) {
                try {
                    return std::stoi(m.at(key));
                } catch (const std::exception& e) {
                    std::cerr << "Error converting '" << m.at(key) << "' to int for key '" << key << "': " << e.what() << std::endl;
                }
            }
            return default_val;
        };

        total_sessions_label->set_text(std::to_string(get_int_or_default(stat, "total_sessions")));
        total_duration_label->set_text(std::to_string(get_int_or_default(stat, "total_duration")) + "分");
        total_calories_label->set_text(std::to_string(get_int_or_default(stat, "total_calories")) + "kcal");
    }
    
    // Get favorite exercise
    std::ostringstream fav_oss;
    fav_oss << "SELECT e.name, COUNT(*) as count "
            << "FROM exercise_history eh "
            << "JOIN exercises e ON eh.exercise_id = e.id "
            << "WHERE eh.user_id = " << m_user.get_id() << " "
            << "GROUP BY e.id, e.name "
            << "ORDER BY count DESC "
            << "LIMIT 1";
    
    auto favorites = global_db_manager->fetch_statistics(fav_oss.str());
    if (!favorites.empty()) {
        favorite_exercise_label->set_text(favorites[0].at("name"));
    } else {
        favorite_exercise_label->set_text("なし");
    }
}

void MyPageWindow::on_edit_profile_clicked() {
    m_signal_edit_profile_request.emit(m_user);
}

void MyPageWindow::on_change_password_clicked() {
    m_signal_change_password_request.emit();
}

void MyPageWindow::on_view_history_clicked() {
    if (!m_history_window) {
        m_history_window = std::make_unique<ExerciseHistoryWindow>(*this);
        m_history_window->set_transient_for(*this);
        m_history_window->set_modal(true); // Make it modal to ensure focus
        
        // Connect to the hide signal to reset the unique_ptr when the window is closed
        m_history_window->signal_hide().connect([this]() {
            if (m_history_window) {
                m_history_window->hide();
                // Resetting the unique_ptr ensures the window is destroyed and recreated fresh next time
                m_history_window.reset(); 
            }
            // Ensure MyPageWindow is focused after the modal closes
            // present(); // Removed: MyPageWindow should already be visible
        });
    }
    m_history_window->present(); // Show the window
}

void MyPageWindow::on_back_clicked() {
    hide();
}

void MyPageWindow::on_logout_clicked() {
    AuthManager::getInstance().logout();
    hide();
}

bool MyPageWindow::on_delete_event(GdkEventAny* event) {
    // Just hide the window instead of destroying it
    hide();
    return true; // Indicate that the event has been handled
}

sigc::signal<void()>& MyPageWindow::signal_change_password_request() {
    return m_signal_change_password_request;
}

sigc::signal<void(const User&)>& MyPageWindow::signal_edit_profile_request() {
    return m_signal_edit_profile_request;
}
