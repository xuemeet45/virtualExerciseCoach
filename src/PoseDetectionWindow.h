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

    const Exercise& exercise;
    Gtk::Box main_box;
    Gtk::Label title_label;
    Gtk::Label status_label;
    Gtk::Button close_button;
    Gtk::ScrolledWindow scrolled_window;
    Gtk::DrawingArea drawing_area;
    
    cv::VideoCapture cap;
    cv::Mat frame;
    bool is_running;
    sigc::connection timeout_connection;
    Glib::RefPtr<Gdk::Pixbuf> current_pixbuf;
    std::vector<guchar> image_buffer;
    std::unique_ptr<tflite::FlatBufferModel> tflite_model;
    std::unique_ptr<tflite::Interpreter> tflite_interpreter;
};

#endif // POSE_DETECTION_WINDOW_H 