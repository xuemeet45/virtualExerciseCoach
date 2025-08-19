#include "ProfileEditWindow.h"
#include "AuthManager.h"
#include "DatabaseManager.h"
#include <iostream>

extern AuthManager* global_auth_manager;
extern DatabaseManager* global_db_manager;

ProfileEditWindow::ProfileEditWindow(Gtk::Window& parent, const User& user)
    : m_label_username("ユーザー名:"),
      m_entry_username(),
      m_label_email("メールアドレス:"),
      m_entry_email(),
      m_label_first_name("名:"),
      m_entry_first_name(),
      m_label_last_name("姓:"),
      m_entry_last_name(),
      m_button_save("保存"),
      m_button_cancel("キャンセル"),
      m_parent_window(parent),
      m_current_user(user)
{
    set_title("ユーザー情報編集");
    set_default_size(400, 300);
    set_modal(true);
    set_transient_for(m_parent_window);

    m_grid.set_row_spacing(10);
    m_grid.set_column_spacing(10);
    m_grid.set_margin(20);

    m_entry_username.set_text(m_current_user.get_username());
    m_entry_email.set_text(m_current_user.get_email());
    m_entry_first_name.set_text(m_current_user.get_first_name());
    m_entry_last_name.set_text(m_current_user.get_last_name());

    // Username is usually not editable, or requires special handling.
    // For now, let's make it read-only.
    m_entry_username.set_editable(false);

    m_grid.attach(m_label_username, 0, 0, 1, 1);
    m_grid.attach(m_entry_username, 1, 0, 1, 1);
    m_grid.attach(m_label_email, 0, 1, 1, 1);
    m_grid.attach(m_entry_email, 1, 1, 1, 1);
    m_grid.attach(m_label_first_name, 0, 2, 1, 1);
    m_grid.attach(m_entry_first_name, 1, 2, 1, 1);
    m_grid.attach(m_label_last_name, 0, 3, 1, 1);
    m_grid.attach(m_entry_last_name, 1, 3, 1, 1);
    m_grid.attach(m_button_save, 0, 4, 1, 1);
    m_grid.attach(m_button_cancel, 1, 4, 1, 1);

    m_button_save.signal_clicked().connect(sigc::mem_fun(*this, &ProfileEditWindow::on_save_button_clicked));
    m_button_cancel.signal_clicked().connect(sigc::mem_fun(*this, &ProfileEditWindow::on_cancel_button_clicked));

    set_child(m_grid);
    show();
}

ProfileEditWindow::~ProfileEditWindow() {}

sigc::signal<void(const User&)>& ProfileEditWindow::signal_profile_update_success() {
    return m_signal_profile_update_success;
}

sigc::signal<void()>& ProfileEditWindow::signal_back_to_mypage() {
    return m_signal_back_to_mypage;
}

void ProfileEditWindow::on_save_button_clicked() {
    std::string new_email = m_entry_email.get_text();
    std::string new_first_name = m_entry_first_name.get_text();
    std::string new_last_name = m_entry_last_name.get_text();

    // Basic validation
    if (new_email.empty() || new_first_name.empty() || new_last_name.empty()) {
        auto dialog = Gtk::make_managed<Gtk::MessageDialog>(m_parent_window, "エラー", false, Gtk::MessageType::ERROR, Gtk::ButtonsType::OK, true);
        dialog->set_secondary_text("すべてのフィールドを入力してください。");
        dialog->signal_response().connect([dialog](int response_id) {
            dialog->hide();
        });
        dialog->present();
        return;
    }

    if (global_db_manager) {
        // Create a temporary User object with updated information
        User updated_user(m_current_user.get_id(), m_current_user.get_username(),
                          new_email, m_current_user.get_password_hash(),
                          new_first_name, new_last_name,
                          m_current_user.get_created_at(), m_current_user.get_last_login(),
                          m_current_user.get_is_active());

        if (global_db_manager->update_user_profile(updated_user)) {
            m_current_user = updated_user; // Update the internal user object
            auto dialog = Gtk::make_managed<Gtk::MessageDialog>(m_parent_window, "ユーザー情報が編集しました。", false, Gtk::MessageType::INFO, Gtk::ButtonsType::OK, true);
            dialog->signal_response().connect([this, dialog](int response_id) {
                if (response_id == Gtk::ResponseType::OK) {
                    dialog->hide();
                    m_signal_profile_update_success.emit(m_current_user);
                }
            });
            dialog->present();
        } else {
            auto dialog = Gtk::make_managed<Gtk::MessageDialog>(m_parent_window, "エラー", false, Gtk::MessageType::ERROR, Gtk::ButtonsType::OK, true);
            dialog->set_secondary_text("ユーザー情報の編集に失敗しました。");
            dialog->signal_response().connect([dialog](int response_id) {
                dialog->hide();
            });
            dialog->present();
        }
    } else {
        auto dialog = Gtk::make_managed<Gtk::MessageDialog>(m_parent_window, "エラー", false, Gtk::MessageType::ERROR, Gtk::ButtonsType::OK, true);
        dialog->set_secondary_text("データベースマネージャーが初期化されていません。");
        dialog->signal_response().connect([dialog](int response_id) {
            dialog->hide();
        });
        dialog->present();
    }
}

void ProfileEditWindow::on_cancel_button_clicked() {
    m_signal_back_to_mypage.emit();
    hide();
}
