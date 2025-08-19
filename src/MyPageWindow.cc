#include "MyPageWindow.h"
#include "AuthManager.h"
#include "DatabaseManager.h"
#include <iostream>

MyPageWindow::MyPageWindow(Gtk::Window& parent) // Accept parent parameter
    : main_box(Gtk::Orientation::VERTICAL),
      profile_box(Gtk::Orientation::VERTICAL),
      stats_box(Gtk::Orientation::VERTICAL),
      actions_box(Gtk::Orientation::VERTICAL) {
    
    set_title("バーチャルエクササイズコーチ - マイページ");
    set_default_size(600, 500);
    set_modal(true);
    set_deletable(true);
    set_resizable(true);
    set_transient_for(parent); // Set MyPageWindow as transient for the main window
    
    // Profile section
    profile_title.set_markup("<big><b>プロフィール</b></big>");
    profile_title.set_halign(Gtk::Align::CENTER);
    profile_title.set_margin_bottom(10);
    
    username_label.set_text("ユーザー名: ");
    username_label.set_halign(Gtk::Align::START);
    username_label.set_margin_start(20);
    username_label.set_margin_end(20);
    
    email_label.set_text("メールアドレス: ");
    email_label.set_halign(Gtk::Align::START);
    email_label.set_margin_start(20);
    email_label.set_margin_end(20);
    
    name_label.set_text("名前: ");
    name_label.set_halign(Gtk::Align::START);
    name_label.set_margin_start(20);
    name_label.set_margin_end(20);
    
    join_date_label.set_text("登録日: ");
    join_date_label.set_halign(Gtk::Align::START);
    join_date_label.set_margin_start(20);
    join_date_label.set_margin_end(20);
    
    last_login_label.set_text("最終ログイン: ");
    last_login_label.set_halign(Gtk::Align::START);
    last_login_label.set_margin_start(20);
    last_login_label.set_margin_end(20);
    last_login_label.set_margin_bottom(20);
    
    // Statistics section
    stats_title.set_markup("<big><b>統計</b></big>");
    stats_title.set_halign(Gtk::Align::CENTER);
    stats_title.set_margin_bottom(10);
    
    total_sessions_label.set_text("総セッション数: 0");
    total_sessions_label.set_halign(Gtk::Align::START);
    total_sessions_label.set_margin_start(20);
    total_sessions_label.set_margin_end(20);
    
    total_duration_label.set_text("総運動時間: 0分");
    total_duration_label.set_halign(Gtk::Align::START);
    total_duration_label.set_margin_start(20);
    total_duration_label.set_margin_end(20);
    
    total_calories_label.set_text("総消費カロリー: 0kcal");
    total_calories_label.set_halign(Gtk::Align::START);
    total_calories_label.set_margin_start(20);
    total_calories_label.set_margin_end(20);
    
    favorite_exercise_label.set_text("お気に入りエクササイズ: なし");
    favorite_exercise_label.set_halign(Gtk::Align::START);
    favorite_exercise_label.set_margin_start(20);
    favorite_exercise_label.set_margin_end(20);
    favorite_exercise_label.set_margin_bottom(20);
    
    // Action buttons
    edit_profile_button.set_label("プロフィール編集");
    edit_profile_button.set_margin_start(20);
    edit_profile_button.set_margin_end(20);
    edit_profile_button.set_margin_bottom(5);
    
    change_password_button.set_label("パスワード変更");
    change_password_button.set_margin_start(20);
    change_password_button.set_margin_end(20);
    change_password_button.set_margin_bottom(5);
    
    view_history_button.set_label("運動履歴");
    view_history_button.set_margin_start(20);
    view_history_button.set_margin_end(20);
    view_history_button.set_margin_bottom(5);
    
    back_button.set_label("戻る");
    back_button.set_margin_start(20);
    back_button.set_margin_end(20);
    back_button.set_margin_bottom(5);
    
    logout_button.set_label("ログアウト");
    logout_button.set_margin_start(20);
    logout_button.set_margin_end(20);
    logout_button.set_margin_bottom(20);
    
    // Layout
    profile_box.set_margin(10);
    profile_box.set_spacing(5);
    profile_box.append(profile_title);
    profile_box.append(username_label);
    profile_box.append(email_label);
    profile_box.append(name_label);
    profile_box.append(join_date_label);
    profile_box.append(last_login_label);
    
    stats_box.set_margin(10);
    stats_box.set_spacing(5);
    stats_box.append(stats_title);
    stats_box.append(total_sessions_label);
    stats_box.append(total_duration_label);
    stats_box.append(total_calories_label);
    stats_box.append(favorite_exercise_label);
    
    actions_box.set_margin(10);
    actions_box.set_spacing(5);
    actions_box.append(edit_profile_button);
    actions_box.append(change_password_button);
    actions_box.append(view_history_button);
    actions_box.append(back_button);
    actions_box.append(logout_button);
    
    main_box.set_margin(10);
    main_box.set_spacing(10);
    main_box.append(profile_box);
    main_box.append(stats_box);
    main_box.append(actions_box);
    
    set_child(main_box);
    
    // Connect signals
    edit_profile_button.signal_clicked().connect(sigc::mem_fun(*this, &MyPageWindow::on_edit_profile_clicked));
    change_password_button.signal_clicked().connect(sigc::mem_fun(*this, &MyPageWindow::on_change_password_clicked));
    view_history_button.signal_clicked().connect(sigc::mem_fun(*this, &MyPageWindow::on_view_history_clicked));
    back_button.signal_clicked().connect(sigc::mem_fun(*this, &MyPageWindow::on_back_clicked));
    logout_button.signal_clicked().connect(sigc::mem_fun(*this, &MyPageWindow::on_logout_clicked));
    
    set_visible(true);
}

void MyPageWindow::updateUserInfo(const User& user) {
    current_user = user;
    
    username_label.set_text("ユーザー名: " + user.get_username());
    email_label.set_text("メールアドレス: " + user.get_email());
    name_label.set_text("名前: " + user.get_last_name() + " " + user.get_first_name());
    join_date_label.set_text("登録日: " + user.get_created_at());
    last_login_label.set_text("最終ログイン: " + user.get_last_login());
    
    refreshStatistics();
}

void MyPageWindow::refreshStatistics() {
    if (!global_db_manager || current_user.get_id() == 0) {
        return;
    }
    
    // Get exercise history statistics
    std::ostringstream oss;
    oss << "SELECT COUNT(*) as total_sessions, "
        << "COALESCE(SUM(duration_minutes), 0) as total_duration, "
        << "COALESCE(SUM(calories_burned), 0) as total_calories "
        << "FROM exercise_history WHERE user_id = " << current_user.get_id();
    
    auto stats = global_db_manager->fetch_statistics(oss.str());
    if (!stats.empty()) {
        const auto& stat = stats[0];
        total_sessions_label.set_text("総セッション数: " + stat.at("total_sessions"));
        total_duration_label.set_text("総運動時間: " + stat.at("total_duration") + "分");
        total_calories_label.set_text("総消費カロリー: " + stat.at("total_calories") + "kcal");
    }
    
    // Get favorite exercise
    std::ostringstream fav_oss;
    fav_oss << "SELECT e.name, COUNT(*) as count "
            << "FROM exercise_history eh "
            << "JOIN exercises e ON eh.exercise_id = e.id "
            << "WHERE eh.user_id = " << current_user.get_id() << " "
            << "GROUP BY e.id, e.name "
            << "ORDER BY count DESC "
            << "LIMIT 1";
    
    auto favorites = global_db_manager->fetch_statistics(fav_oss.str());
    if (!favorites.empty()) {
        favorite_exercise_label.set_text("お気に入りエクササイズ: " + favorites[0].at("name"));
    } else {
        favorite_exercise_label.set_text("お気に入りエクササイズ: なし");
    }
}

void MyPageWindow::on_edit_profile_clicked() {
    // TODO: Implement profile editing
    std::cout << "Edit profile clicked" << std::endl;
}

void MyPageWindow::on_change_password_clicked() {
    m_signal_change_password_request.emit();
}

void MyPageWindow::on_view_history_clicked() {
    // TODO: Implement history view
    std::cout << "View history clicked" << std::endl;
}

void MyPageWindow::on_back_clicked() {
    hide();
}

void MyPageWindow::on_logout_clicked() {
    AuthManager::getInstance().logout();
    hide();
}

sigc::signal<void()>& MyPageWindow::signal_change_password_request() {
    return m_signal_change_password_request;
}
