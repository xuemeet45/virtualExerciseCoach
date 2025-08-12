#include "RegisterWindow.h"
#include "AuthManager.h"
#include <iostream>
#include <regex>

RegisterWindow::RegisterWindow()
    : main_box(Gtk::Orientation::VERTICAL),
      form_box(Gtk::Orientation::VERTICAL) {
    
    set_title("バーチャルエクササイズコーチ - 新規登録");
    set_default_size(400, 500);
    set_modal(true);
    set_deletable(false);
    set_resizable(false);
    
    // Center the window
    set_halign(Gtk::Align::CENTER);
    set_valign(Gtk::Align::CENTER);
    
    // Title
    title_label.set_markup("<big><b>新規登録</b></big>");
    title_label.set_halign(Gtk::Align::CENTER);
    title_label.set_margin_bottom(20);
    
    // Form elements
    username_label.set_text("ユーザー名:");
    username_label.set_halign(Gtk::Align::START);
    username_label.set_margin_start(20);
    username_label.set_margin_end(20);
    
    username_entry.set_placeholder_text("ユーザー名を入力してください");
    username_entry.set_margin_start(20);
    username_entry.set_margin_end(20);
    username_entry.set_margin_bottom(10);
    
    email_label.set_text("メールアドレス:");
    email_label.set_halign(Gtk::Align::START);
    email_label.set_margin_start(20);
    email_label.set_margin_end(20);
    
    email_entry.set_placeholder_text("メールアドレスを入力してください");
    email_entry.set_margin_start(20);
    email_entry.set_margin_end(20);
    email_entry.set_margin_bottom(10);
    
    password_label.set_text("パスワード:");
    password_label.set_halign(Gtk::Align::START);
    password_label.set_margin_start(20);
    password_label.set_margin_end(20);
    
    password_entry.set_placeholder_text("パスワードを入力してください");
    password_entry.set_visibility(false);
    password_entry.set_margin_start(20);
    password_entry.set_margin_end(20);
    password_entry.set_margin_bottom(10);
    
    confirm_password_label.set_text("パスワード確認:");
    confirm_password_label.set_halign(Gtk::Align::START);
    confirm_password_label.set_margin_start(20);
    confirm_password_label.set_margin_end(20);
    
    confirm_password_entry.set_placeholder_text("パスワードを再入力してください");
    confirm_password_entry.set_visibility(false);
    confirm_password_entry.set_margin_start(20);
    confirm_password_entry.set_margin_end(20);
    confirm_password_entry.set_margin_bottom(10);
    
    first_name_label.set_text("名:");
    first_name_label.set_halign(Gtk::Align::START);
    first_name_label.set_margin_start(20);
    first_name_label.set_margin_end(20);
    
    first_name_entry.set_placeholder_text("名を入力してください");
    first_name_entry.set_margin_start(20);
    first_name_entry.set_margin_end(20);
    first_name_entry.set_margin_bottom(10);
    
    last_name_label.set_text("姓:");
    last_name_label.set_halign(Gtk::Align::START);
    last_name_label.set_margin_start(20);
    last_name_label.set_margin_end(20);
    
    last_name_entry.set_placeholder_text("姓を入力してください");
    last_name_entry.set_margin_start(20);
    last_name_entry.set_margin_end(20);
    last_name_entry.set_margin_bottom(20);
    
    // Buttons
    register_button.set_label("登録");
    register_button.set_margin_start(20);
    register_button.set_margin_end(20);
    register_button.set_margin_bottom(10);
    
    back_button.set_label("戻る");
    back_button.set_margin_start(20);
    back_button.set_margin_end(20);
    back_button.set_margin_bottom(10);
    
    // Status label
    status_label.set_text("");
    status_label.set_halign(Gtk::Align::CENTER);
    status_label.set_margin_start(20);
    status_label.set_margin_end(20);
    
    // Layout
    form_box.set_margin(20);
    form_box.set_spacing(5);
    form_box.append(title_label);
    form_box.append(username_label);
    form_box.append(username_entry);
    form_box.append(email_label);
    form_box.append(email_entry);
    form_box.append(password_label);
    form_box.append(password_entry);
    form_box.append(confirm_password_label);
    form_box.append(confirm_password_entry);
    form_box.append(first_name_label);
    form_box.append(first_name_entry);
    form_box.append(last_name_label);
    form_box.append(last_name_entry);
    form_box.append(register_button);
    form_box.append(back_button);
    form_box.append(status_label);
    
    main_box.append(form_box);
    set_child(main_box);
    
    // Connect signals
    register_button.signal_clicked().connect(sigc::mem_fun(*this, &RegisterWindow::on_register_clicked));
    back_button.signal_clicked().connect(sigc::mem_fun(*this, &RegisterWindow::on_back_clicked));
    
    set_visible(true);
}

void RegisterWindow::on_register_clicked() {
    if (!validate_form()) {
        return;
    }
    
    Glib::ustring username = username_entry.get_text();
    Glib::ustring email = email_entry.get_text();
    Glib::ustring password = password_entry.get_text();
    Glib::ustring first_name = first_name_entry.get_text();
    Glib::ustring last_name = last_name_entry.get_text();
    
    // Attempt registration
    if (AuthManager::getInstance().registerUser(username, email, password, first_name, last_name)) {
        status_label.set_text("登録が完了しました！");
        status_label.set_markup("<span foreground='green'>登録が完了しました！</span>");
        
        // Emit success signal
        signal_register_success.emit();
        
        // Close the register window
        hide();
    } else {
        status_label.set_text("登録に失敗しました。ユーザー名またはメールアドレスが既に使用されています。");
        status_label.set_markup("<span foreground='red'>登録に失敗しました。ユーザー名またはメールアドレスが既に使用されています。</span>");
    }
}

void RegisterWindow::on_back_clicked() {
    signal_back_to_login.emit();
    hide();
}

bool RegisterWindow::validate_form() {
    Glib::ustring username = username_entry.get_text();
    Glib::ustring email = email_entry.get_text();
    Glib::ustring password = password_entry.get_text();
    Glib::ustring confirm_password = confirm_password_entry.get_text();
    Glib::ustring first_name = first_name_entry.get_text();
    Glib::ustring last_name = last_name_entry.get_text();
    
    // Check if all fields are filled
    if (username.empty() || email.empty() || password.empty() || 
        confirm_password.empty() || first_name.empty() || last_name.empty()) {
        status_label.set_text("すべての項目を入力してください");
        status_label.set_markup("<span foreground='red'>すべての項目を入力してください</span>");
        return false;
    }
    
    // Check username length
    if (username.length() < 3) {
        status_label.set_text("ユーザー名は3文字以上で入力してください");
        status_label.set_markup("<span foreground='red'>ユーザー名は3文字以上で入力してください</span>");
        return false;
    }
    
    // Check email format
    std::regex email_regex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    if (!std::regex_match(email.raw(), email_regex)) {
        status_label.set_text("有効なメールアドレスを入力してください");
        status_label.set_markup("<span foreground='red'>有効なメールアドレスを入力してください</span>");
        return false;
    }
    
    // Check password length
    if (password.length() < 6) {
        status_label.set_text("パスワードは6文字以上で入力してください");
        status_label.set_markup("<span foreground='red'>パスワードは6文字以上で入力してください</span>");
        return false;
    }
    
    // Check password confirmation
    if (password != confirm_password) {
        status_label.set_text("パスワードが一致しません");
        status_label.set_markup("<span foreground='red'>パスワードが一致しません</span>");
        return false;
    }
    
    return true;
} 