#include <gtkmm/application.h>
#include "VirtualFitnessCoachWindow.h"
#include "DatabaseManager.h"
#include "LoginWindow.h"
#include "RegisterWindow.h"
#include "AuthManager.h"

extern AuthManager* global_auth_manager; // Declare global_auth_manager

class VirtualFitnessCoachApp : public Gtk::Application {
protected:
    VirtualFitnessCoachApp() : Gtk::Application("org.example.VirtualFitnessCoach") {}

    Gtk::ApplicationWindow* create_appwindow() {
        auto window = new VirtualFitnessCoachWindow();
        add_window(*window);
        
        // Connect the logout signal
        window->signal_logout_request.connect([this, window]() {
            window->hide(); // Hide the main application window
            show_login_window(); // Show the login window again
        });
        
        window->present();
        return window;
    }

    void on_activate() override {
        show_login_window();
    }
    
    void show_login_window() {
        auto login_window = new LoginWindow();
        add_window(*login_window);
        
        login_window->signal_login_success.connect([this, login_window]() {
            login_window->hide();
            create_appwindow();
        });
        
        login_window->signal_register_request.connect([this, login_window]() {
            login_window->hide();
            show_register_window();
        });
        
        login_window->present();
    }
    
    void show_register_window() {
        auto register_window = new RegisterWindow();
        add_window(*register_window);
        
        register_window->signal_register_success.connect([this, register_window]() {
            register_window->hide();
            show_login_window();
        });
        
        register_window->signal_back_to_login.connect([this, register_window]() {
            register_window->hide();
            show_login_window();
        });
        
        register_window->present();
    }

public:
    static Glib::RefPtr<VirtualFitnessCoachApp> create() {
        return Glib::make_refptr_for_instance<VirtualFitnessCoachApp>(new VirtualFitnessCoachApp());
    }
};

int main(int argc, char* argv[]) {
    // Initialize the global DatabaseManager instance
    global_db_manager = new DatabaseManager("dbname=fitnessdb user=admin password=admin hostaddr=127.0.0.1 port=5432");
    
    // Initialize the global AuthManager instance
    global_auth_manager = &AuthManager::getInstance();

    auto app = VirtualFitnessCoachApp::create();
    int result = app->run(argc, argv);

    // Clean up the global DatabaseManager instance
    delete global_db_manager;
    // No need to delete global_auth_manager as it's a static singleton

    return result;
}
