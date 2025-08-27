#include "ExerciseHistoryWindow.h"
#include <iostream>
#include <iomanip> // For std::put_time
#include <ctime>   // For std::localtime, std::mktime

extern DatabaseManager* global_db_manager;
extern AuthManager* global_auth_manager;

// --- ExerciseHistoryWindow Implementation ---
ExerciseHistoryWindow::ExerciseHistoryWindow(Gtk::Window& parent)
    : Gtk::Window(), // Default constructor for Gtk::Window
      m_main_box(Gtk::Orientation::VERTICAL),
      m_title_label("運動履歴"),
      m_back_button("戻る") {
    
    std::cout << "ExerciseHistoryWindow constructor called." << std::endl;

    set_title("バーチャルエクササイズコーチ - 運動履歴");
    set_default_size(800, 600);
    set_modal(true);
    set_deletable(true); // Allow closing with window decorations
    set_resizable(true);
    set_transient_for(parent); // Set MyPageWindow as transient for the main window

    // Removed override_background_color; relying on CSS for styling
    // Removed add_css_class("window"); from the window itself.
    // Relying on CSS for background styling.

    m_main_box.set_margin(20);
    m_main_box.set_spacing(15);
    m_main_box.set_hexpand(true); // Ensure main box expands horizontally
    m_main_box.set_vexpand(true); // Ensure main box expands vertically
    // Removed set_halign and set_valign to allow full expansion
    // Removed temporary override_background_color

    m_title_label.add_css_class("mypage-section-title");
    m_title_label.set_halign(Gtk::Align::CENTER);
    m_main_box.append(m_title_label);

    // Setup ListBox
    m_scrolled_window.set_expand(true);
    m_scrolled_window.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    m_scrolled_window.set_child(m_history_list_box);
    m_main_box.append(m_scrolled_window);

    m_history_list_box.set_selection_mode(Gtk::SelectionMode::NONE); // No selection for history items
    m_history_list_box.add_css_class("mypage-box-frame"); // Apply frame style to the listbox itself

    m_back_button.add_css_class("mypage-button");
    m_back_button.set_halign(Gtk::Align::CENTER);
    m_back_button.set_size_request(150, -1);
    m_main_box.append(m_back_button);

    m_back_button.signal_clicked().connect(sigc::mem_fun(*this, &ExerciseHistoryWindow::on_back_button_clicked));

    set_child(m_main_box);
}

ExerciseHistoryWindow::~ExerciseHistoryWindow() {}

void ExerciseHistoryWindow::on_show() {
    Gtk::Window::on_show();
    std::cout << "ExerciseHistoryWindow on_show called." << std::endl;
    load_history_data();
}

void ExerciseHistoryWindow::on_back_button_clicked() {
    hide();
}

void ExerciseHistoryWindow::load_history_data() {
    std::cout << "ExerciseHistoryWindow load_history_data called." << std::endl;
    if (!global_db_manager || !global_auth_manager || !global_auth_manager->isLoggedIn()) {
        std::cerr << "ERROR: Cannot load history: User not logged in or DB manager not initialized." << std::endl;
        return;
    }

    // Clear existing items
    m_history_list_box.remove_all();

    int user_id = global_auth_manager->getCurrentUser().get_id();
    auto history_records = global_db_manager->fetch_exercise_history(user_id);

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

    for (const auto& record : history_records) {
        auto item_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
        item_box->set_spacing(10);
        item_box->set_margin(5);
        item_box->add_css_class("mypage-box-frame"); // Reuse frame style for list items

        auto exercise_name_label = Gtk::make_managed<Gtk::Label>(record.count("exercise_name") ? record.at("exercise_name") : "N/A");
        auto date_label = Gtk::make_managed<Gtk::Label>();
        auto duration_label = Gtk::make_managed<Gtk::Label>();
        auto status_label = Gtk::make_managed<Gtk::Label>(record.count("status") ? record.at("status") : "N/A");

        exercise_name_label->add_css_class("mypage-label");
        date_label->add_css_class("mypage-label");
        duration_label->add_css_class("mypage-label");
        status_label->add_css_class("mypage-label");

        exercise_name_label->set_hexpand(true);
        date_label->set_hexpand(true);
        duration_label->set_hexpand(true);
        status_label->set_hexpand(true);

        // Format session_date
        if (record.count("session_date")) {
            std::string date_str = record.at("session_date");
            size_t space_pos = date_str.find(' ');
            if (space_pos != std::string::npos) {
                date_label->set_text(date_str.substr(0, space_pos));
            } else {
                date_label->set_text(date_str);
            }
        } else {
            date_label->set_text("N/A");
        }

        std::string duration_text = "N/A";
        int seconds = get_int_or_default(record, "performed_seconds", -1);
        if (seconds != -1) {
            int minutes = seconds / 60;
            int remaining_seconds = seconds % 60;
            duration_text = std::to_string(minutes) + "m " + std::to_string(remaining_seconds) + "s";
        } else { // Fallback to old column if new one not present
            int minutes = get_int_or_default(record, "duration_minutes", -1);
            if (minutes != -1) {
                duration_text = std::to_string(minutes) + "m";
            }
        }
        if (duration_text == "N/A") {
            duration_label->set_text("-");
        } else {
            duration_label->set_text(duration_text);
        }

        if (record.count("status")) {
            if (record.at("status") == "実行") {
                status_label->add_css_class("color-success");
                status_label->remove_css_class("color-primary");
                status_label->remove_css_class("color-info");
            } else if (record.at("status") == "確認") {
                status_label->add_css_class("color-info"); // Use info color for '確認'
                status_label->remove_css_class("color-primary");
                status_label->remove_css_class("color-success");
            }
            else { // Default to primary color for '試行'
                status_label->add_css_class("color-primary");
                status_label->remove_css_class("color-success");
                status_label->remove_css_class("color-info");
            }
        }

        item_box->append(*exercise_name_label);
        item_box->append(*date_label);
        item_box->append(*duration_label);
        item_box->append(*status_label);
        
        m_history_list_box.append(*item_box);
    }
}
