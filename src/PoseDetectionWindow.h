#ifndef POSE_DETECTION_WINDOW_H
#define POSE_DETECTION_WINDOW_H

#include <gtkmm.h>
#include <opencv2/opencv.hpp>
#include "Exercise.h"
#include <vector>
#include <memory>
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/model.h"

class PoseDetectionWindow : public Gtk::Window {
public:
    PoseDetectionWindow(const Exercise& exercise);
    ~PoseDetectionWindow() override;

protected:
    void on_realize() override;
    void on_unrealize() override;
    void on_show() override;
    void on_hide() override;

private:
    bool update_frame();
    void start_camera();
    void stop_camera();
    void on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);

    void record_reference_pose(); // Add this method
    bool on_countdown_timer(); // Countdown timer callback
    bool on_recording_timer(); // Recording timer callback

    const Exercise& exercise;
    Gtk::Box main_box;
    Gtk::Label title_label;
    Gtk::Label status_label;
    Gtk::Button close_button;
    Gtk::Button record_button;
    Gtk::ScrolledWindow scrolled_window;
    Gtk::DrawingArea drawing_area;

    std::string current_error_message; // To display one error at a time
    std::vector<std::string> error_message_queue; // To queue up multiple error messages
    void update_status_label(const std::string& message, const std::string& color = "black", int font_size = 20);
    
    cv::VideoCapture cap;
    cv::Mat frame;
    bool is_running;
    sigc::connection timeout_connection;
    Glib::RefPtr<Gdk::Pixbuf> current_pixbuf;
    std::vector<guchar> image_buffer;
    std::unique_ptr<tflite::FlatBufferModel> tflite_model;
    std::unique_ptr<tflite::Interpreter> tflite_interpreter;

    std::vector<float> last_detected_keypoints; // Store last detected keypoints
    
    // Timer and recording state variables
    sigc::connection countdown_connection;
    sigc::connection recording_connection;
    int countdown_value;
    int recording_duration;
    bool is_counting_down;
    bool is_recording;
    std::vector<std::vector<float>> recorded_poses; // Store recorded poses during recording period
};

#endif // POSE_DETECTION_WINDOW_H
