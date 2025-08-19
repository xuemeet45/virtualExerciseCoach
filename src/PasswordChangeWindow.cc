#include "PasswordChangeWindow.h"
#include "AuthManager.h"
#include "DatabaseManager.h"
#include <iostream>

extern AuthManager* global_auth_manager;
extern DatabaseManager* global_db_manager;

PasswordChangeWindow::PasswordChangeWindow(Gtk::Window& parent) // Accept parent parameter
    : m_label_old_password("現在のパスワード:"),
      m_entry_old_password(),
      m_label_new_password("新しいパスワード:"),
      m_entry_new_password(),
      m_label_confirm_new_password("新しいパスワード再入力:"),
      m_entry_confirm_new_password(),
      m_checkbutton_show_password("パスワードを表示する"), // Initialize new check button
      m_button_change_password("パスワードを変更する"),
      m_button_cancel("戻る"),
      m_parent_window(parent) // Initialize m_parent_window
{
    set_title("パスワード変更");
    set_default_size(300, 200);
    set_modal(true);
    set_transient_for(m_parent_window); // Use the stored parent for transient

    m_grid.set_row_spacing(10);
    m_grid.set_column_spacing(10);
    m_grid.set_margin(20);

    m_entry_old_password.set_visibility(false);
    m_entry_new_password.set_visibility(false);
    m_entry_confirm_new_password.set_visibility(false);

    m_grid.attach(m_label_old_password, 0, 0, 1, 1);
    m_grid.attach(m_entry_old_password, 1, 0, 1, 1);
    m_grid.attach(m_label_new_password, 0, 1, 1, 1);
    m_grid.attach(m_entry_new_password, 1, 1, 1, 1);
    m_grid.attach(m_label_confirm_new_password, 0, 2, 1, 1);
    m_grid.attach(m_entry_confirm_new_password, 1, 2, 1, 1);
    m_grid.attach(m_checkbutton_show_password, 1, 3, 1, 1); // Attach show password checkbox
    m_grid.attach(m_button_change_password, 0, 4, 1, 1);
    m_grid.attach(m_button_cancel, 1, 4, 1, 1);

    m_button_change_password.signal_clicked().connect(sigc::mem_fun(*this, &PasswordChangeWindow::on_change_password_button_clicked));
    m_button_cancel.signal_clicked().connect(sigc::mem_fun(*this, &PasswordChangeWindow::on_cancel_button_clicked));
    m_checkbutton_show_password.signal_toggled().connect([this]() {
        bool visible = m_checkbutton_show_password.get_active();
        m_entry_old_password.set_visibility(visible);
        m_entry_new_password.set_visibility(visible);
        m_entry_confirm_new_password.set_visibility(visible);
    });

    set_child(m_grid); // In GTKMM4, Gtk::Window uses set_child()
    show();
}

PasswordChangeWindow::~PasswordChangeWindow() {}

sigc::signal<void()>& PasswordChangeWindow::signal_password_change_success() {
    return m_signal_password_change_success;
}

sigc::signal<void()>& PasswordChangeWindow::signal_back_to_mypage() {
    return m_signal_back_to_mypage;
}

void PasswordChangeWindow::on_change_password_button_clicked() {
    std::string old_password = m_entry_old_password.get_text();
    std::string new_password = m_entry_new_password.get_text();
    std::string confirm_new_password = m_entry_confirm_new_password.get_text();

    std::cout << "Attempting password change:" << std::endl;
    std::cout << "Old Password: " << old_password << std::endl;
    std::cout << "New Password: " << new_password << std::endl;
    std::cout << "Confirm New Password: " << confirm_new_password << std::endl;

    if (new_password != confirm_new_password) {
        auto dialog = Gtk::make_managed<Gtk::MessageDialog>(m_parent_window, "エラー", false, Gtk::MessageType::ERROR, Gtk::ButtonsType::OK, true);
        dialog->set_secondary_text("新しいパスワードが一致しません。");
        dialog->signal_response().connect([dialog](int response_id) {
            dialog->hide();
        });
        dialog->present();
        return;
    }

    if (new_password.empty()) {
        auto dialog = Gtk::make_managed<Gtk::MessageDialog>(m_parent_window, "エラー", false, Gtk::MessageType::ERROR, Gtk::ButtonsType::OK, true);
        dialog->set_secondary_text("新しいパスワードは空にできません。");
        dialog->signal_response().connect([dialog](int response_id) {
            dialog->hide();
        });
        dialog->present();
        return;
    }

    if (global_auth_manager && global_db_manager) {
        int user_id = global_auth_manager->get_current_user_id();
        std::cout << "Current User ID: " << user_id << std::endl;
        if (user_id != -1) {
            if (global_auth_manager->change_password(user_id, old_password, new_password)) {
                std::cout << "Password change successful!" << std::endl;
                auto dialog = Gtk::make_managed<Gtk::MessageDialog>(m_parent_window, "成功", false, Gtk::MessageType::INFO, Gtk::ButtonsType::OK, true);
                dialog->set_secondary_text("パスワードが正常に変更されました！");
                dialog->signal_response().connect([this, dialog](int response_id) {
                    if (response_id == Gtk::ResponseType::OK) {
                        dialog->hide();
                        m_signal_password_change_success.emit();
                        // Do NOT hide() PasswordChangeWindow here. Let VirtualFitnessCoachWindow handle it.
                    }
                });
                dialog->present();
            } else {
                std::cout << "Password change failed." << std::endl;
                auto dialog = Gtk::make_managed<Gtk::MessageDialog>(m_parent_window, "エラー", false, Gtk::MessageType::ERROR, Gtk::ButtonsType::OK, true);
                dialog->set_secondary_text("パスワードの変更に失敗しました。現在のパスワードを確認してください。");
                dialog->signal_response().connect([dialog](int response_id) {
                    dialog->hide();
                });
                dialog->present();
            }
        } else {
            std::cout << "No user logged in." << std::endl;
            auto dialog = Gtk::make_managed<Gtk::MessageDialog>(m_parent_window, "エラー", false, Gtk::MessageType::ERROR, Gtk::ButtonsType::OK, true);
            dialog->set_secondary_text("ログインしているユーザーがいません。");
            dialog->signal_response().connect([dialog](int response_id) {
                dialog->hide();
            });
            dialog->present();
        }
    } else {
        std::cout << "AuthManager or DatabaseManager not initialized." << std::endl;
        auto dialog = Gtk::make_managed<Gtk::MessageDialog>(m_parent_window, "エラー", false, Gtk::MessageType::ERROR, Gtk::ButtonsType::OK, true);
        dialog->set_secondary_text("認証マネージャーまたはデータベースマネージャーが初期化されていません。");
        dialog->signal_response().connect([dialog](int response_id) {
            dialog->hide();
        });
        dialog->present();
    }
}

void PasswordChangeWindow::on_cancel_button_clicked() {
    m_signal_back_to_mypage.emit();
    hide();
}
