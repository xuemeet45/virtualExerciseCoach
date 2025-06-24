#include <gtkmm/application.h>
#include "VirtualFitnessCoachWindow.h"
#include "DatabaseManager.h"

class VirtualFitnessCoachApp : public Gtk::Application {
protected:
    VirtualFitnessCoachApp() : Gtk::Application("org.example.VirtualFitnessCoach") {}

    Gtk::ApplicationWindow* create_appwindow() {
        auto window = new VirtualFitnessCoachWindow();
        add_window(*window);
        window->present();
        return window;
    }

    void on_activate() override {
        create_appwindow();
    }

public:
    static Glib::RefPtr<VirtualFitnessCoachApp> create() {
        return Glib::make_refptr_for_instance<VirtualFitnessCoachApp>(new VirtualFitnessCoachApp());
    }
};

int main(int argc, char* argv[]) {
    // Initialize the global DatabaseManager instance
    global_db_manager = new DatabaseManager("dbname=fitnessdb user=admin password=admin hostaddr=127.0.0.1 port=5432");

    auto app = VirtualFitnessCoachApp::create();
    int result = app->run(argc, argv);

    // Clean up the global DatabaseManager instance
    delete global_db_manager;

    return result;
}
