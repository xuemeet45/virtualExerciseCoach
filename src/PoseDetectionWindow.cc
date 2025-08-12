#include "PoseDetectionWindow.h"
#include <opencv2/opencv.hpp>
#include <gtkmm.h>
#include <iostream>
#include <memory>
#include <tensorflow/lite/interpreter.h>
#include <tensorflow/lite/kernels/register.h>
#include <tensorflow/lite/model.h>
#include <tensorflow/lite/optional_debug_tools.h>
#include "DatabaseManager.h" // Add this include
#include <algorithm>
#include <sstream> // Added for std::ostringstream

const char* joint_names[17] = {
    "Nose", "Left Eye", "Right Eye", "Left Ear", "Right Ear",
    "Left Shoulder", "Right Shoulder", "Left Elbow", "Right Elbow",
    "Left Wrist", "Right Wrist", "Left Hip", "Right Hip",
    "Left Knee", "Right Knee", "Left Ankle", "Right Ankle"
};

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
      is_running(false),
      countdown_value(0),
      recording_duration(0),
      is_counting_down(false),
      is_recording(false) {
    
    set_title("バーチャルエクササイズコーチ - ポーズ検出");
    set_default_size(800, 600);
    set_modal(true);
    set_deletable(true);
    set_resizable(true);

    // Create UI elements
    title_label.set_markup("<big><b>" + exercise.get_name() + "</b></big>");
    status_label.set_text("Initializing camera...");
    close_button.set_label("Close");
    record_button.set_label("Record Reference Pose");

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
    main_box.append(record_button);
    main_box.append(close_button);

    // Connect signals
    close_button.signal_clicked().connect([this]() {
        // Disconnect any active timers
        if (countdown_connection.connected()) {
            countdown_connection.disconnect();
        }
        if (recording_connection.connected()) {
            recording_connection.disconnect();
        }
        stop_camera();
        hide();
    });
    record_button.signal_clicked().connect(sigc::mem_fun(*this, &PoseDetectionWindow::record_reference_pose));

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
    // Disconnect any active timers
    if (countdown_connection.connected()) {
        countdown_connection.disconnect();
    }
    if (recording_connection.connected()) {
        recording_connection.disconnect();
    }
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
                            // Store last detected keypoints for recording
                            last_detected_keypoints.clear();
                            for (int i = 0; i < num_keypoints * keypoint_size; ++i) {
                                last_detected_keypoints.push_back(output_data[i]);
                            }
                            // --- Advanced feedback: highlight incorrect joints and show per-joint feedback ---
                            auto ref_keypoints = global_db_manager->fetch_reference_pose(exercise.get_id());
                            bool has_reference = false;
                            for (const auto& kp : ref_keypoints) {
                                if (kp[2] > 0.01) { has_reference = true; break; }
                            }
                            std::vector<int> incorrect_joints;
                            float total_error = 0.0f;
                            int count = 0;
                            if (has_reference && last_detected_keypoints.size() == 17 * 3) {
                                for (int i = 0; i < 17; ++i) {
                                    float user_x = last_detected_keypoints[i * 3 + 1];
                                    float user_y = last_detected_keypoints[i * 3];
                                    float ref_x = ref_keypoints[i][0];
                                    float ref_y = ref_keypoints[i][1];
                                    float dx = user_x - ref_x;
                                    float dy = user_y - ref_y;
                                    float dist = std::sqrt(dx * dx + dy * dy);
                                    total_error += dist;
                                    count++;
                                    if (dist > 0.05) { // Threshold for incorrect joint
                                        incorrect_joints.push_back(i);
                                    }
                                }
                                float avg_error = (count > 0) ? (total_error / count) : 0.0f;
                                // Draw keypoints with color feedback
                                for (int i = 0; i < num_keypoints; i++) {
                                    float y = output_data[i * keypoint_size];
                                    float x = output_data[i * keypoint_size + 1];
                                    float confidence = output_data[i * keypoint_size + 2];
                                    int px = static_cast<int>(x * 640);
                                    int py = static_cast<int>(y * 480);
                                    cv::Point keypoint(px, py);
                                    bool is_incorrect = std::find(incorrect_joints.begin(), incorrect_joints.end(), i) != incorrect_joints.end();
                                    cv::Scalar color = is_incorrect ? cv::Scalar(0, 0, 255) : cv::Scalar(0, 255, 0); // Red or Green
                                    cv::circle(frame, keypoint, 5, color, -1);
                                    cv::putText(frame, std::to_string(i), keypoint + cv::Point(5, 5), 
                                                cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
                                }
                                // Show per-joint feedback in the status label
                                if (!incorrect_joints.empty()) {
                                    std::string feedback = "Adjust: ";
                                    for (size_t j = 0; j < incorrect_joints.size(); ++j) {
                                        feedback += joint_names[incorrect_joints[j]];
                                        if (j + 1 < incorrect_joints.size()) feedback += ", ";
                                    }
                                    status_label.set_text(feedback);
                                } else {
                                    status_label.set_text("Great! All joints match.");
                                }
                            } else if (!has_reference) {
                                status_label.set_text("No reference pose recorded for this exercise.");
                            }
                            // --- End advanced feedback ---
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
                            int frame_number = 0; // Define frame_number for static pose
                            for (int i = 0; i < num_keypoints; ++i) {
                                float x = output_data[i * keypoint_size];
                                float y = output_data[i * keypoint_size + 1];
                                float confidence = output_data[i * keypoint_size + 2];
                                global_db_manager->insert_exercise_pose(exercise.get_id(), i, x, y, confidence, frame_number);
                            }
                            
                            // --- Pose comparison and feedback ---
                            // This section is now handled by the advanced feedback logic above
                            // auto ref_keypoints = global_db_manager->fetch_reference_pose(exercise.get_id());
                            // bool has_reference = false;
                            // for (const auto& kp : ref_keypoints) {
                            //     if (kp[2] > 0.01) { has_reference = true; break; }
                            // }
                            // if (has_reference && last_detected_keypoints.size() == 17 * 3) {
                            //     float total_error = 0.0f;
                            //     int count = 0;
                            //     for (int i = 0; i < 17; ++i) {
                            //         float user_x = last_detected_keypoints[i * 3 + 1];
                            //         float user_y = last_detected_keypoints[i * 3];
                            //         float ref_x = ref_keypoints[i][0];
                            //         float ref_y = ref_keypoints[i][1];
                            //         float dx = user_x - ref_x;
                            //         float dy = user_y - ref_y;
                            //         float dist = std::sqrt(dx * dx + dy * dy);
                            //         total_error += dist;
                            //         count++;
                            //     }
                            //     float avg_error = (count > 0) ? (total_error / count) : 0.0f;
                            //     if (avg_error < 0.05) {
                            //         status_label.set_text("Good job! Pose matches reference.");
                            //     } else {
                            //         status_label.set_text("Adjust your pose to match the reference.");
                            //     }
                            // } else if (!has_reference) {
                            //     status_label.set_text("No reference pose recorded for this exercise.");
                            // }
                            // --- End pose comparison and feedback ---
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

    // Add countdown and recording overlay
    if (is_counting_down) {
        // Draw countdown overlay
        cv::putText(frame, "Get Ready!", 
                   cv::Point(frame.cols/2 - 100, frame.rows/2 - 50), 
                   cv::FONT_HERSHEY_SIMPLEX, 1.5, cv::Scalar(255, 255, 0), 3);
        cv::putText(frame, std::to_string(countdown_value), 
                   cv::Point(frame.cols/2 - 30, frame.rows/2 + 20), 
                   cv::FONT_HERSHEY_SIMPLEX, 3.0, cv::Scalar(255, 0, 0), 4);
    } else if (is_recording) {
        // Draw recording overlay
        cv::putText(frame, "RECORDING", 
                   cv::Point(frame.cols/2 - 80, frame.rows/2 - 50), 
                   cv::FONT_HERSHEY_SIMPLEX, 1.2, cv::Scalar(0, 0, 255), 3);
        
        // Draw recording progress
        int seconds = recording_duration / 1000;
        int tenths = (recording_duration % 1000) / 100;
        std::string time_text = std::to_string(seconds) + "." + std::to_string(tenths) + "s";
        cv::putText(frame, time_text, 
                   cv::Point(frame.cols/2 - 40, frame.rows/2 + 20), 
                   cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 2);
        
        // Draw recording indicator (red circle)
        cv::circle(frame, cv::Point(frame.cols - 50, 50), 15, cv::Scalar(0, 0, 255), -1);
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

void PoseDetectionWindow::record_reference_pose() {
    // Check if a reference pose already exists for this exercise
    std::ostringstream oss;
    oss << "SELECT COUNT(*) FROM reference_poses WHERE exercise_id = " << exercise.get_id();
    auto stats = global_db_manager->fetch_statistics(oss.str());
    bool already_exists = false;
    if (!stats.empty() && stats[0].count("count") > 0) {
        already_exists = (std::stoi(stats[0].at("count")) > 0);
    }

    if (already_exists) {
        auto dialog = new Gtk::MessageDialog(
            *this,
            "参照ポーズがすでに存在します。再登録しますか？",
            false, // use_markup
            Gtk::MessageType::QUESTION // Set dialog type (INFO, WARNING, QUESTION, etc.)
        );
        dialog->set_secondary_text("以前の参照ポーズが削除します。よろしいでしょうか？");
        dialog->set_modal(true);
        dialog->set_default_response(Gtk::ResponseType::NO);

        // Remove all default buttons by only adding your own
        dialog->add_button("戻る", Gtk::ResponseType::NO);
        dialog->add_button("はい", Gtk::ResponseType::YES);

        dialog->signal_response().connect(
            [this, dialog](int response_id) {
                if (response_id == Gtk::ResponseType::YES) {
                    // Proceed with recording
                    record_button.set_sensitive(false);
                    countdown_value = 5;
                    is_counting_down = true;
                    is_recording = false;
                    recorded_poses.clear();
                    countdown_connection = Glib::signal_timeout().connect(
                        sigc::mem_fun(*this, &PoseDetectionWindow::on_countdown_timer), 1000);
                    status_label.set_text("Get ready! Recording in " + std::to_string(countdown_value) + "...");
                }
                dialog->hide();
            }
        );
        dialog->show();
        return;
    }

    // No reference pose exists, proceed directly
    record_button.set_sensitive(false);
    countdown_value = 5;
    is_counting_down = true;
    is_recording = false;
    recorded_poses.clear();
    countdown_connection = Glib::signal_timeout().connect(
        sigc::mem_fun(*this, &PoseDetectionWindow::on_countdown_timer), 1000);
    status_label.set_text("Get ready! Recording in " + std::to_string(countdown_value) + "...");
}

bool PoseDetectionWindow::on_countdown_timer() {
    countdown_value--;
    
    if (countdown_value > 0) {
        status_label.set_text("Get ready! Recording in " + std::to_string(countdown_value) + "...");
        return true; // Continue countdown
    } else {
        // Countdown finished, start recording
        is_counting_down = false;
        is_recording = true;
        recording_duration = 0;
        recorded_poses.clear();
        
        status_label.set_text("Recording... (0/5 seconds)");
        
        // Start recording timer (update every 100ms for smoother progress)
        recording_connection = Glib::signal_timeout().connect(
            sigc::mem_fun(*this, &PoseDetectionWindow::on_recording_timer), 100);
        
        return false; // Stop countdown timer
    }
}

bool PoseDetectionWindow::on_recording_timer() {
    recording_duration += 100; // Increment by 100ms
    
    if (recording_duration < 5000) { // Less than 5 seconds
        // Record current pose if available
        if (last_detected_keypoints.size() == 17 * 3) {
            recorded_poses.push_back(last_detected_keypoints);
        }
        
        int seconds = recording_duration / 1000;
        int tenths = (recording_duration % 1000) / 100;
        status_label.set_text("Recording... (" + std::to_string(seconds) + "." + std::to_string(tenths) + "/5.0 seconds)");
        
        return true; // Continue recording
    } else {
        // Recording finished
        is_recording = false;
        
        // Process recorded poses
        if (recorded_poses.empty()) {
            status_label.set_text("No poses recorded. Please try again.");
            record_button.set_sensitive(true);
            return false;
        }
        
        // Use the middle pose as reference (most stable)
        int middle_index = recorded_poses.size() / 2;
        const std::vector<float>& reference_pose = recorded_poses[middle_index];

        // Always delete old reference poses for this exercise before inserting new ones
        {
            std::ostringstream oss;
            oss << "DELETE FROM reference_poses WHERE exercise_id = " << exercise.get_id();
            global_db_manager->execute_query(oss.str());
        }

        // Save the reference pose
        bool all_success = true;
        for (int i = 0; i < 17; ++i) {
            float y = reference_pose[i * 3];
            float x = reference_pose[i * 3 + 1];
            float confidence = reference_pose[i * 3 + 2];
            // frame_number = 0 for static reference pose
            if (!global_db_manager->insert_reference_pose(exercise.get_id(), i, x, y, confidence, 0)) {
                all_success = false;
            }
        }
        
        if (all_success) {
            status_label.set_text("Reference pose recorded successfully! (" + std::to_string(recorded_poses.size()) + " poses captured)");
        } else {
            status_label.set_text("Failed to record reference pose.");
        }
        
        record_button.set_sensitive(true);
        return false; // Stop recording timer
    }
}
