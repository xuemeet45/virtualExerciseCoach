#include "LoginWindow.h"
#include "AuthManager.h"
#include <iostream>

LoginWindow::LoginWindow()
    : main_box(Gtk::Orientation::VERTICAL),
      form_box(Gtk::Orientation::VERTICAL) {
    
    set_title("バーチャルエクササイズコーチ - ログイン");
    set_modal(true);
    set_deletable(false);
    set_resizable(true); // Allow resizing for full width
    
    // Align to fill the available space
    set_halign(Gtk::Align::FILL);
    set_valign(Gtk::Align::FILL);
    
    // Title
    title_label.set_markup("<big><b>ログイン</b></big>");
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
    
    password_label.set_text("パスワード:");
    password_label.set_halign(Gtk::Align::START);
    password_label.set_margin_start(20);
    password_label.set_margin_end(20);
    
    password_entry.set_placeholder_text("パスワードを入力してください");
    password_entry.set_visibility(false);
    password_entry.set_margin_start(20);
    password_entry.set_margin_end(20);
    password_entry.set_margin_bottom(20);
    
    // Buttons
    login_button.set_label("ログイン");
    login_button.set_margin_start(20);
    login_button.set_margin_end(20);
    login_button.set_margin_bottom(10);
    
    register_button.set_label("新規登録");
    register_button.set_margin_start(20);
    register_button.set_margin_end(20);
    register_button.set_margin_bottom(10);
    
    guest_button.set_label("ゲストとして続行");
    guest_button.set_margin_start(20);
    guest_button.set_margin_end(20);
    guest_button.set_margin_bottom(10);

    // Remember me checkbox
    remember_me_checkbutton.set_label("IDとパスワードを記憶する");
    remember_me_checkbutton.set_margin_start(20);
    remember_me_checkbutton.set_margin_end(20);
    remember_me_checkbutton.set_margin_bottom(10);
    
    // Status label
    status_label.set_text("");
    status_label.set_halign(Gtk::Align::CENTER);
    status_label.set_margin_start(20);
    status_label.set_margin_end(20);
    
    // Layout
    form_box.set_margin(20);
    form_box.set_spacing(5);
    form_box.set_halign(Gtk::Align::CENTER); // Center the form within the window
    form_box.set_valign(Gtk::Align::CENTER);
    form_box.append(title_label);
    form_box.append(username_label);
    form_box.append(username_entry);
    form_box.append(password_label);
    form_box.append(password_entry);
    form_box.append(remember_me_checkbutton); // Add remember me checkbox
    form_box.append(login_button);
    form_box.append(register_button);
    form_box.append(guest_button);
    form_box.append(status_label);
    
    main_box.set_halign(Gtk::Align::FILL); // Make main_box fill horizontally
    main_box.set_valign(Gtk::Align::FILL); // Make main_box fill vertically
    main_box.append(form_box);
    set_child(main_box);
    
    // Connect signals
    login_button.signal_clicked().connect(sigc::mem_fun(*this, &LoginWindow::on_login_clicked));
    register_button.signal_clicked().connect(sigc::mem_fun(*this, &LoginWindow::on_register_clicked));
    guest_button.signal_clicked().connect(sigc::mem_fun(*this, &LoginWindow::on_guest_clicked));
    
    // Allow Enter key to trigger login
    username_entry.signal_activate().connect(sigc::mem_fun(*this, &LoginWindow::on_login_clicked));
    password_entry.signal_activate().connect(sigc::mem_fun(*this, &LoginWindow::on_login_clicked));

    // Load saved credentials
    Glib::RefPtr<Glib::KeyFile> key_file = Glib::KeyFile::create();
    try {
        key_file->load_from_data(Glib::file_get_contents("login_credentials.conf"));
        Glib::ustring saved_username = key_file->get_string("Login", "username");
        Glib::ustring saved_password = key_file->get_string("Login", "password");
        bool remember_me = key_file->get_boolean("Login", "remember_me");

        if (remember_me) {
            username_entry.set_text(saved_username);
            password_entry.set_text(saved_password);
            remember_me_checkbutton.set_active(true);
        }
    } catch (const Glib::FileError& ex) {
        // File not found or other error, ignore and proceed
        std::cerr << "Error loading login credentials: " << ex.what() << std::endl;
    }
    
    set_visible(true);
}

void LoginWindow::on_login_clicked() {
    Glib::ustring username = username_entry.get_text();
    Glib::ustring password = password_entry.get_text();
    
    if (username.empty() || password.empty()) {
        status_label.set_text("ユーザー名とパスワードを入力してください");
        status_label.set_markup("<span foreground='red'>ユーザー名とパスワードを入力してください</span>");
        return;
    }
    
    // Attempt authentication
    if (AuthManager::getInstance().authenticateUser(username, password)) {
        status_label.set_text("ログイン成功！");
        status_label.set_markup("<span foreground='green'>ログイン成功！</span>");
        
        // Save credentials if "remember me" is checked
        if (remember_me_checkbutton.get_active()) {
            Glib::RefPtr<Glib::KeyFile> key_file = Glib::KeyFile::create();
            key_file->set_string("Login", "username", username);
            key_file->set_string("Login", "password", password);
            key_file->set_boolean("Login", "remember_me", true);
            Glib::file_set_contents("login_credentials.conf", key_file->to_data());
        } else {
            // Clear saved credentials if "remember me" is unchecked
            Glib::file_set_contents("login_credentials.conf", "");
        }
        
        // Emit success signal
        signal_login_success.emit();
        
        // Close the login window
        hide();
    } else {
        status_label.set_text("ログインに失敗しました。ユーザー名とパスワードを確認してください。");
        status_label.set_markup("<span foreground='red'>ログインに失敗しました。ユーザー名とパスワードを確認してください。</span>");
        password_entry.set_text("");
    }
}

void LoginWindow::on_register_clicked() {
    signal_register_request.emit();
}

void LoginWindow::on_guest_clicked() {
    // For guest mode, we'll just emit login success without authentication
    signal_login_success.emit();
    hide();
}
