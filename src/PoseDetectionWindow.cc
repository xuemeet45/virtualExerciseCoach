#include "PoseDetectionWindow.h"
#include <opencv2/opencv.hpp>
#include <gtkmm.h>
#include <iostream>
#include <memory>
#include <tensorflow/lite/interpreter.h>
#include <tensorflow/lite/kernels/register.h>
#include <tensorflow/lite/model.h>
#include <tensorflow/lite/optional_debug_tools.h>

// Helper function to draw pose skeleton
void draw_pose_skeleton(cv::Mat& frame, float* keypoints) {
    // Define skeleton connections for MoveNet (17 keypoints)
    // Each pair represents indices of connected keypoints
    const std::vector<std::pair<int, int>> skeleton = {
        {0, 1},   // nose to left_eye
        {0, 2},   // nose to right_eye
        {1, 3},   // left_eye to left_ear
        {2, 4},   // right_eye to right_ear
        {5, 6},   // left_shoulder to right_shoulder
        {5, 7},   // left_shoulder to left_elbow
        {7, 9},   // left_elbow to left_wrist
        {6, 8},   // right_shoulder to right_elbow
        {8, 10},  // right_elbow to right_wrist
        {5, 11},  // left_shoulder to left_hip
        {6, 12},  // right_shoulder to right_hip
        {11, 12}, // left_hip to right_hip
        {11, 13}, // left_hip to left_knee
        {13, 15}, // left_knee to left_ankle
        {12, 14}, // right_hip to right_knee
        {14, 16}  // right_knee to right_ankle
    };
    
    for (const auto& connection : skeleton) {
        int idx1 = connection.first;
        int idx2 = connection.second;
        
        if (idx1 * 3 + 2 < 51 && idx2 * 3 + 2 < 51) { // 17 keypoints * 3 values
            float y1 = keypoints[idx1 * 3] * 480;
            float x1 = keypoints[idx1 * 3 + 1] * 640;
            float conf1 = keypoints[idx1 * 3 + 2];
            
            float y2 = keypoints[idx2 * 3] * 480;
            float x2 = keypoints[idx2 * 3 + 1] * 640;
            float conf2 = keypoints[idx2 * 3 + 2];
            
            // Only draw connection if both keypoints are confident
            if (conf1 > 0.01 && conf2 > 0.01 && 
                x1 >= 0 && x1 < 640 && y1 >= 0 && y1 < 480 &&
                x2 >= 0 && x2 < 640 && y2 >= 0 && y2 < 480) {
                cv::line(frame, cv::Point(x1, y1), cv::Point(x2, y2), 
                        cv::Scalar(255, 0, 0), 2);
            }
        }
    }
}

PoseDetectionWindow::PoseDetectionWindow(const Exercise& exercise)
    : Gtk::Window(),
      main_box(Gtk::Orientation::VERTICAL),
      exercise(exercise),
      is_running(false) {
    
    set_title("バーチャルエクササイズコーチ - ポーズ検出");
    set_default_size(800, 600);
    set_modal(true);
    set_deletable(true);
    set_resizable(true);

    // Create UI elements
    title_label.set_markup("<big><b>" + exercise.get_name() + "</b></big>");
    status_label.set_text("Initializing camera...");
    close_button.set_label("Close");

    // Configure the drawing area
    drawing_area.set_size_request(640, 480);
    drawing_area.set_halign(Gtk::Align::CENTER);
    drawing_area.set_valign(Gtk::Align::CENTER);
    drawing_area.set_expand(true);
    drawing_area.set_visible(true);  // Explicitly make it visible

    // Connect the draw signal
    drawing_area.set_draw_func(sigc::mem_fun(*this, &PoseDetectionWindow::on_draw));

    // Create a scrolled window for the drawing area
    scrolled_window.set_expand(true);
    scrolled_window.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    scrolled_window.set_child(drawing_area);
    scrolled_window.set_visible(true);  // Explicitly make it visible

    // Set up layout
    main_box.set_margin(10);
    main_box.set_spacing(10);
    main_box.append(title_label);
    main_box.append(status_label);
    main_box.append(scrolled_window);
    main_box.append(close_button);

    // Connect signals
    close_button.signal_clicked().connect([this]() {
        stop_camera();
        hide();
    });

    set_child(main_box);
    set_visible(true);

    // Load TFLite model
    try {
        std::cout << "Loading TFLite model..." << std::endl;
        tflite_model = tflite::FlatBufferModel::BuildFromFile("movenet_singlepose_lightning.tflite");
        if (!tflite_model) {
            std::cerr << "Failed to load TFLite model" << std::endl;
            return;
        }
        
        std::cout << "Creating TFLite interpreter..." << std::endl;
        tflite::ops::builtin::BuiltinOpResolver resolver;
        tflite::InterpreterBuilder builder(*tflite_model, resolver);
        builder(&tflite_interpreter);
        
        if (!tflite_interpreter) {
            std::cerr << "Failed to create TFLite interpreter" << std::endl;
            return;
        }
        
        if (tflite_interpreter->AllocateTensors() != kTfLiteOk) {
            std::cerr << "Failed to allocate tensors" << std::endl;
            return;
        }
        
        // Print model info for debugging
        std::cout << "Model loaded successfully!" << std::endl;
        std::cout << "Input tensors: " << tflite_interpreter->inputs().size() << std::endl;
        std::cout << "Output tensors: " << tflite_interpreter->outputs().size() << std::endl;
        
        // Print input tensor info
        TfLiteTensor* input_tensor = tflite_interpreter->input_tensor(0);
        if (input_tensor) {
            std::cout << "Input tensor type: " << input_tensor->type << std::endl;
            std::cout << "Input tensor dimensions: ";
            for (int i = 0; i < input_tensor->dims->size; i++) {
                std::cout << input_tensor->dims->data[i] << " ";
            }
            std::cout << std::endl;
        }
        
        // Print output tensor info
        TfLiteTensor* output_tensor = tflite_interpreter->output_tensor(0);
        if (output_tensor) {
            std::cout << "Output tensor type: " << output_tensor->type << std::endl;
            std::cout << "Output tensor dimensions: ";
            for (int i = 0; i < output_tensor->dims->size; i++) {
                std::cout << output_tensor->dims->data[i] << " ";
            }
            std::cout << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading TFLite model: " << e.what() << std::endl;
    }
}

PoseDetectionWindow::~PoseDetectionWindow() {
    stop_camera();
}

void PoseDetectionWindow::on_realize() {
    Gtk::Window::on_realize();
    start_camera();
}

void PoseDetectionWindow::on_unrealize() {
    stop_camera();
    Gtk::Window::on_unrealize();
}

void PoseDetectionWindow::on_show() {
    Gtk::Window::on_show();
    if (!is_running) {
        std::cout << "Window shown, starting camera..." << std::endl;
        start_camera();
    }
}

void PoseDetectionWindow::on_hide() {
    Gtk::Window::on_hide();
    std::cout << "Window hidden, stopping camera..." << std::endl;
    stop_camera();
}

void PoseDetectionWindow::start_camera() {
    if (!is_running) {
        std::cout << "Attempting to open camera..." << std::endl;
        // Initialize camera
        cap.open(0);
        if (!cap.isOpened()) {
            std::cerr << "Error: Could not open camera" << std::endl;
            status_label.set_text("Error: Could not open camera");
            return;
        }

        std::cout << "Camera opened successfully" << std::endl;

        // Set camera properties
        cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
        cap.set(cv::CAP_PROP_FPS, 30);

        is_running = true;
        std::cout << "Starting frame update timer..." << std::endl;
        timeout_connection = Glib::signal_timeout().connect(
            sigc::mem_fun(*this, &PoseDetectionWindow::update_frame), 33); // ~30 FPS
    }
}

void PoseDetectionWindow::stop_camera() {
    if (is_running) {
        std::cout << "Stopping camera..." << std::endl;
        timeout_connection.disconnect();
        cap.release();
        is_running = false;
    }
}

bool PoseDetectionWindow::update_frame() {
    if (!is_running) return false;
    cap >> frame;
    if (frame.empty()) return false;

    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
    cv::resize(frame, frame, cv::Size(640, 480));

    // Pose detection if TFLite model is loaded
    if (tflite_interpreter && tflite_model) {
        try {
            std::cout << "Running pose detection..." << std::endl;
            
            // Get input tensor
            TfLiteTensor* input_tensor = tflite_interpreter->input_tensor(0);
            std::cout << "Input tensor pointer: " << input_tensor << std::endl;
            
            if (input_tensor) {
                std::cout << "Input tensor type: " << input_tensor->type << std::endl;
                std::cout << "Input tensor dimensions: ";
                for (int i = 0; i < input_tensor->dims->size; i++) {
                    std::cout << input_tensor->dims->data[i] << " ";
                }
                std::cout << std::endl;
                
                // Check if dimensions are reasonable for MoveNet
                if (input_tensor->dims->size != 4) {
                    std::cerr << "Invalid tensor dimensions: expected 4D tensor" << std::endl;
                    return true;
                }
                
                int batch_size = input_tensor->dims->data[0];
                int height = input_tensor->dims->data[1];
                int width = input_tensor->dims->data[2];
                int channels = input_tensor->dims->data[3];
                
                std::cout << "Batch: " << batch_size << ", Height: " << height 
                         << ", Width: " << width << ", Channels: " << channels << std::endl;
                
                if (channels != 3) {
                    std::cerr << "Invalid channels: expected 3, got " << channels << std::endl;
                    return true;
                }
                
                // Handle both quantized (uint8) and float32 models
                if (input_tensor->type == kTfLiteFloat32) {
                    std::cout << "Processing float32 model" << std::endl;
                    
                    // Preprocess frame for TFLite model
                    cv::Mat rgb_frame;
                    cv::cvtColor(frame, rgb_frame, cv::COLOR_BGR2RGB);
                    cv::Mat input_frame;
                    cv::resize(rgb_frame, input_frame, cv::Size(width, height));
                    cv::Mat float_frame;
                    input_frame.convertTo(float_frame, CV_32F, 1.0/255.0);
                    
                    // Copy preprocessed frame to input tensor
                    float* input_data = input_tensor->data.f;
                    for (int y = 0; y < height; y++) {
                        for (int x = 0; x < width; x++) {
                            cv::Vec3f pixel = float_frame.at<cv::Vec3f>(y, x);
                            int idx = (y * width + x) * 3;
                            input_data[idx] = pixel[0];     // R
                            input_data[idx + 1] = pixel[1]; // G
                            input_data[idx + 2] = pixel[2]; // B
                        }
                    }
                    
                    // Print input tensor min/max for debugging
                    float min_val = 1e6, max_val = -1e6;
                    for (int i = 0; i < width * height * channels; ++i) {
                        float v = input_data[i];
                        if (v < min_val) min_val = v;
                        if (v > max_val) max_val = v;
                    }
                    std::cout << "Input tensor min: " << min_val << ", max: " << max_val << std::endl;
                    
                } else if (input_tensor->type == kTfLiteUInt8) {
                    std::cout << "Processing quantized (uint8) model" << std::endl;
                    
                    // Preprocess frame for quantized model
                    cv::Mat input_frame;
                    cv::resize(frame, input_frame, cv::Size(width, height));
                    
                    // Copy preprocessed frame to input tensor (uint8)
                    uint8_t* input_data = input_tensor->data.uint8;
                    for (int y = 0; y < height; y++) {
                        for (int x = 0; x < width; x++) {
                            cv::Vec3b pixel = input_frame.at<cv::Vec3b>(y, x);
                            int idx = (y * width + x) * 3;
                            input_data[idx] = pixel[0];     // R
                            input_data[idx + 1] = pixel[1]; // G
                            input_data[idx + 2] = pixel[2]; // B
                        }
                    }
                    
                } else {
                    std::cerr << "Unsupported tensor type: " << input_tensor->type << std::endl;
                    return true;
                }
                
                std::cout << "Input data copied, running inference..." << std::endl;
                
                // Run inference
                if (tflite_interpreter->Invoke() == kTfLiteOk) {
                    std::cout << "Inference successful" << std::endl;
                    
                    // Get output tensor (MoveNet outputs keypoints)
                    TfLiteTensor* output_tensor = tflite_interpreter->output_tensor(0);
                    std::cout << "Output tensor pointer: " << output_tensor << std::endl;
                    
                    if (output_tensor) {
                        std::cout << "Output tensor type: " << output_tensor->type << std::endl;
                        std::cout << "Output tensor dimensions: ";
                        for (int i = 0; i < output_tensor->dims->size; i++) {
                            std::cout << output_tensor->dims->data[i] << " ";
                        }
                        std::cout << std::endl;
                        
                        if (output_tensor->type == kTfLiteFloat32) {
                            float* output_data = output_tensor->data.f;
                            int output_size = output_tensor->dims->data[output_tensor->dims->size - 1];
                            std::cout << "Output size: " << output_size << std::endl;
                            
                            // Correctly parse MoveNet output: [1, 1, 17, 3]
                            int num_keypoints = output_tensor->dims->data[2]; // 17
                            int keypoint_size = output_tensor->dims->data[3]; // 3
                            std::cout << "Drawing " << num_keypoints << " keypoints" << std::endl;
                            std::cout << "Keypoints (y, x, confidence):" << std::endl;
                            for (int i = 0; i < num_keypoints; i++) {
                                float y = output_data[i * keypoint_size];
                                float x = output_data[i * keypoint_size + 1];
                                float confidence = output_data[i * keypoint_size + 2];
                                std::cout << "  " << i << ": y=" << y << ", x=" << x << ", conf=" << confidence << std::endl;
                                if (confidence > 0.3 && x >= 0 && x < 1 && y >= 0 && y < 1) {
                                    int px = static_cast<int>(x * 640);
                                    int py = static_cast<int>(y * 480);
                                    cv::Point keypoint(px, py);
                                    cv::circle(frame, keypoint, 5, cv::Scalar(0, 255, 0), -1);
                                    cv::putText(frame, std::to_string(i), keypoint + cv::Point(5, 5), 
                                              cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
                                }
                            }
                            // Draw skeleton connections
                            draw_pose_skeleton(frame, output_data);
                            std::cout << "Pose detection completed successfully" << std::endl;
                            
                            // Print the full keypoint array for debugging
                            std::cout << "Full keypoint array (y, x, conf): ";
                            for (int i = 0; i < num_keypoints; i++) {
                                float y = output_data[i * keypoint_size];
                                float x = output_data[i * keypoint_size + 1];
                                float confidence = output_data[i * keypoint_size + 2];
                                std::cout << "[" << y << ", " << x << ", " << confidence << "] ";
                            }
                            std::cout << std::endl;
                            
                            // After pose detection and keypoints extraction, insert pose data for each keypoint
                            for (int i = 0; i < num_keypoints; ++i) {
                                float x = output_data[i * keypoint_size];
                                float y = output_data[i * keypoint_size + 1];
                                float confidence = output_data[i * keypoint_size + 2];
                                global_db_manager->insert_exercise_pose(exercise.get_id(), i, x, y, confidence, frame_number);
                            }
                            
                        } else if (output_tensor->type == kTfLiteUInt8) {
                            std::cout << "Processing quantized output tensor" << std::endl;
                            uint8_t* output_data = output_tensor->data.uint8;
                            int output_size = output_tensor->dims->data[output_tensor->dims->size - 1];
                            std::cout << "Output size: " << output_size << std::endl;
                            
                            // For quantized output, we need to dequantize
                            // This is a simplified approach - in practice you'd need proper dequantization
                            std::cout << "First few output values: ";
                            for (int i = 0; i < std::min(10, output_size); i++) {
                                std::cout << (int)output_data[i] << " ";
                            }
                            std::cout << std::endl;
                            
                            // Try to interpret as keypoints (this is experimental)
                            int num_keypoints = std::min(17, output_size / 3);
                            std::cout << "Attempting to draw " << num_keypoints << " keypoints from quantized output" << std::endl;
                            
                            for (int i = 0; i < num_keypoints; i++) {
                                if (i * 3 + 2 < output_size) {
                                    // Simple dequantization (this may not be correct)
                                    float y = (output_data[i * 3] / 255.0f) * 480;
                                    float x = (output_data[i * 3 + 1] / 255.0f) * 640;
                                    float confidence = output_data[i * 3 + 2] / 255.0f;
                                    
                                    if (confidence > 0.3 && x >= 0 && x < 640 && y >= 0 && y < 480) {
                                        cv::Point keypoint(x, y);
                                        cv::circle(frame, keypoint, 5, cv::Scalar(0, 255, 0), -1);
                                        cv::putText(frame, std::to_string(i), keypoint + cv::Point(5, 5), 
                                                  cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
                                    }
                                }
                            }
                            
                            std::cout << "Quantized pose detection completed" << std::endl;
                            
                        } else {
                            std::cerr << "Unsupported output tensor type: " << output_tensor->type << std::endl;
                        }
                    } else {
                        std::cerr << "Invalid output tensor type or null tensor" << std::endl;
                    }
                } else {
                    std::cerr << "TFLite inference failed" << std::endl;
                }
            } else {
                std::cerr << "Invalid input tensor type or null tensor" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Pose detection error: " << e.what() << std::endl;
        }
    } else {
        std::cout << "TFLite model not loaded, running camera test mode" << std::endl;
        // Add a simple text overlay to show camera is working
        cv::putText(frame, "Camera Test Mode - No Pose Detection", 
                   cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1.0, 
                   cv::Scalar(0, 255, 0), 2);
        cv::putText(frame, "Camera is working correctly!", 
                   cv::Point(10, 70), cv::FONT_HERSHEY_SIMPLEX, 0.8, 
                   cv::Scalar(255, 255, 255), 2);
    }

    int width = frame.cols;
    int height = frame.rows;
    int stride = width * 3;
    image_buffer.resize(stride * height);
    if (frame.step == stride) {
        std::memcpy(image_buffer.data(), frame.data, image_buffer.size());
    } else {
        for (int y = 0; y < height; ++y) {
            std::memcpy(image_buffer.data() + y * stride, frame.ptr(y), stride);
        }
    }

    current_pixbuf = Gdk::Pixbuf::create_from_data(
        image_buffer.data(),
        Gdk::Colorspace::RGB,
        false,
        8,
        width,
        height,
        stride
    );

    drawing_area.queue_draw();
    return true;
}

void PoseDetectionWindow::on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height) {
    if (!current_pixbuf) return;

    double scale_x = static_cast<double>(width) / current_pixbuf->get_width();
    double scale_y = static_cast<double>(height) / current_pixbuf->get_height();
    double scale = std::min(scale_x, scale_y);
    double x = (width - current_pixbuf->get_width() * scale) / 2;
    double y = (height - current_pixbuf->get_height() * scale) / 2;

    cr->save();
    cr->translate(x, y);
    cr->scale(scale, scale);
    Gdk::Cairo::set_source_pixbuf(cr, current_pixbuf, 0, 0);
    cr->paint();
    cr->restore();
} 