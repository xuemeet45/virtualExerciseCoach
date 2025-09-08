[日本語のREADMEはこちら](README_ja.md)

# Virtual Exercise Coach (バーチャルエクササイズコーチ)

A modern desktop application for real-time exercise pose detection and coaching with a complete user authentication and management system. Built with C++, GTKmm, OpenCV, TensorFlow Lite, and PostgreSQL.

## 🚀 Features

### 🔐 Authentication & User Management
- **JWT Token Authentication**: Secure token-based authentication using HMAC-SHA256
- **User Registration**: Complete signup process with form validation
- **User Login**: Secure login with password hashing
- **Session Management**: Automatic token validation and cleanup
- **Guest Mode**: Option to use the app without authentication

### 🏃‍♂️ Exercise & Pose Detection
- **Real-time Pose Detection**: Using MoveNet (TensorFlow Lite)
- **Visual Skeleton Overlay**: Real-time keypoint tracking and visualization
- **Reference Pose Recording**: 5-second countdown timer with pose recording
- **Pose Comparison**: Compare current pose with reference poses
- **Exercise Library**: Browse and practice various exercises

### 📊 User Experience
- **MyPage**: User profile and exercise statistics
- **Exercise History**: Track workout sessions and progress
- **Statistics Dashboard**: View total sessions, duration, and calories burned
- **Japanese UI**: Complete interface in Japanese
- **Modern GTK4 Interface**: Clean and responsive design

### 🎯 Exercise Categories
- **Strength Training**: Various strength exercises
- **Yoga Poses**: 10 classic yoga poses with Japanese names (e.g., Tree Pose, Warrior I, Warrior II, Triangle Pose, Mountain Pose)
- **Cardio Exercises**: Cardiovascular workout routines
- **Flexibility Training**: Stretching and flexibility exercises

## 🛠️ Build Instructions

### Prerequisites
- macOS (tested on macOS 14+)
- Homebrew package manager
- PostgreSQL 14+
- PostgreSQL client libraries (e.g., `libpq`)
- OpenSSL 3+
- C++17 compiler (clang++)

### Installation

1. **Install Dependencies**
```bash
# Install required packages via Homebrew
brew install gtkmm4 opencv postgresql@14 openssl@3

# TensorFlow Lite: The project includes prebuilt TensorFlow Lite libraries,
# so no separate installation is required for the library itself.
```

2. **Database Setup**
```bash
# Create PostgreSQL database
createdb fitnessdb

# Run the database schema
psql -d fitnessdb -f database_schema.sql

# Create admin user (optional)
psql -d fitnessdb -f create_admin_user.sql

# Populate with sample yoga exercises (optional)
psql -d fitnessdb -f clear_and_insert_yoga.sql
```
**Default Database Connection (from src/main.cc):**
`dbname=fitnessdb user=admin password=admin hostaddr=127.0.0.1 port=5432`

3. **Build and Run**
```bash
# Using CMake (recommended)
mkdir build
cd build
cmake ..
make
./VirtualExerciseCoach

# Or using the provided script (for quick setup, uses CMake internally)
./compile_and_run_gtkmm.sh
```

## 📋 Database Schema

The application uses PostgreSQL with the following tables:

- **`users`**: User accounts and authentication data
- **`user_sessions`**: JWT token management
- **`user_profiles`**: Additional user information
- **`exercises`**: Exercise library and metadata
- **`exercise_history`**: User workout tracking
- **`exercise_poses`**: Pose data for exercises
- **`reference_poses`**: Reference poses for comparison

## 🔑 Authentication

### Default Admin Account
- **Username**: `admin`
- **Password**: `admin123`

### JWT Token Features
- **24-hour expiration**: Tokens automatically expire
- **HMAC-SHA256 signing**: Secure token validation
- **Session tracking**: Database-stored session management
- **Automatic cleanup**: Expired sessions are invalidated

## 🎮 Usage Guide

### First Time Setup
1. Launch the application
2. Login with admin credentials or create a new account
3. Allow camera access when prompted
4. Navigate to exercise library

### Recording Reference Poses
1. Select an exercise from the library
2. Click "試す" (Try) to open pose detection
3. Click "Record Reference Pose"
4. Follow the 5-second countdown
5. Hold the pose for 5 seconds while recording
6. The reference pose is automatically saved

### Using MyPage
1. Click "マイページ" (MyPage) in the sidebar
2. View your profile information
3. Check exercise statistics
4. Access workout history
5. Manage account settings

## 🏗️ Project Structure

```
gtkmm-sample/
├── src/                          # C++ source files (application logic, UI components, managers)
│   ├── main.cc                   # Application entry point and setup
│   ├── VirtualFitnessCoachWindow.cc  # Main application window and navigation
│   ├── LoginWindow.cc            # User authentication login interface
│   ├── RegisterWindow.cc         # User registration interface
│   ├── MyPageWindow.cc           # User profile and statistics display
│   ├── PoseDetectionWindow.cc    # Real-time pose detection and comparison
│   ├── DatabaseManager.cc        # Handles all PostgreSQL database interactions
│   ├── AuthManager.cc            # Manages JWT token-based authentication
│   └── *.h                       # Corresponding header files for source files
├── models/                       # TensorFlow Lite models (e.g., MoveNet)
│   └── movenet_singlepose_lightning.tflite
├── images/                       # Application images and assets
├── database_schema.sql           # SQL script for setting up the PostgreSQL database schema
├── create_admin_user.sql         # SQL script for creating a default admin user
├── compile_and_run_gtkmm.sh      # Convenience script for building and running the application (uses CMake)
└── CMakeLists.txt                # CMake build configuration file
```

## 🔧 Technical Stack

- **Frontend**: GTKmm 4 (C++)
- **Computer Vision**: OpenCV 4
- **AI/ML**: TensorFlow Lite (MoveNet)
- **Database**: PostgreSQL 14
- **Authentication**: JWT tokens with OpenSSL
- **Build System**: CMake
- **Language**: C++17

## 🐛 Troubleshooting

### Common Issues

1. **Camera Access Denied**
   - Ensure camera permissions are granted for the application.
   - Check if another application is currently using the camera.

2. **Database Connection Failed**
   - Verify PostgreSQL is running: `brew services start postgresql@14`
   - Ensure the `fitnessdb` database exists.
   - Check the database credentials (default: `dbname=fitnessdb user=admin password=admin hostaddr=127.0.0.1 port=5432`).

3. **Compilation Errors**
   - Update Homebrew packages: `brew update && brew upgrade`
   - Ensure `gtkmm4`, `opencv`, `postgresql@14`, and `openssl@3` are correctly installed via Homebrew.
   - Verify C++17 compiler (clang++) is available.

4. **Model Loading Issues**
   - Ensure `movenet_singlepose_lightning.tflite` is present in the `models/` directory.
   - Check file permissions for the model file.

### Performance Optimization
- **GPU Acceleration**: TensorFlow Lite supports GPU delegates for improved performance.
- **Memory Management**: Automatic cleanup of pose detection windows and resources.
- **Database Indexing**: Optimized queries with proper indexes for faster data retrieval.

## 🤝 Contributing

1. Fork the repository.
2. Create a feature branch (`git checkout -b feature/AmazingFeature`).
3. Make your changes.
4. Test thoroughly.
5. Commit your changes (`git commit -m 'Add some AmazingFeature'`).
6. Push to the branch (`git push origin feature/AmazingFeature`).
7. Open a Pull Request.

## 📄 License

Distributed under the MIT License. See the project's GitHub repository for details.

## 🙏 Acknowledgments

- **TensorFlow Lite**: For the MoveNet pose detection model.
- **GTKmm**: For the modern GUI framework.
- **OpenCV**: For computer vision capabilities.
- **PostgreSQL**: For robust database management.
- **OpenSSL**: For cryptographic functions used in JWT.

## 📞 Support

For issues and questions:
1. Check the troubleshooting section above.
2. Review the database schema and ensure proper setup.
3. Verify all dependencies are installed and correctly configured.
4. Create an issue on the GitHub repository.

---

**Note**: This application is designed for educational and fitness purposes. Always consult with a fitness professional before starting any exercise program.
