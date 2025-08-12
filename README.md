[日本語のREADMEはこちら](README_ja.md)

# Virtual Exercise Coach (バーチャルエクササイズコーチ)

A modern desktop application for real-time exercise pose detection and coaching with complete user authentication and management system. Built with C++, GTKmm, OpenCV, TensorFlow Lite, and PostgreSQL.

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
- **Yoga Poses**: 5 classic yoga poses with Japanese names
- **Cardio Exercises**: Cardiovascular workout routines
- **Flexibility Training**: Stretching and flexibility exercises

## 🛠️ Build Instructions

### Prerequisites
- macOS (tested on macOS 14+)
- Homebrew package manager
- PostgreSQL 14+
- C++17 compiler (clang++)

### Installation

1. **Install Dependencies**
```bash
# Install required packages via Homebrew
brew install gtkmm4 opencv postgresql@14 openssl@3

# Install TensorFlow Lite (if not already present)
# The project includes prebuilt TensorFlow Lite libraries
```

2. **Database Setup**
```bash
# Create PostgreSQL database
createdb fitnessdb

# Run the database schema
psql -d fitnessdb -f database_schema.sql

# Create admin user (optional)
psql -d fitnessdb -f create_admin_user.sql
```

3. **Build and Run**
```bash
# Using the provided script (recommended)
./compile_and_run_gtkmm.sh

# Or using CMake
mkdir build
cd build
cmake ..
make
./VirtualExerciseCoach
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
├── src/                          # C++ source files
│   ├── main.cc                   # Application entry point
│   ├── VirtualFitnessCoachWindow.cc  # Main application window
│   ├── LoginWindow.cc            # Authentication login
│   ├── RegisterWindow.cc         # User registration
│   ├── MyPageWindow.cc           # User profile and stats
│   ├── PoseDetectionWindow.cc    # Real-time pose detection
│   ├── DatabaseManager.cc        # Database operations
│   ├── AuthManager.cc            # JWT authentication
│   └── *.h                       # Header files
├── models/                       # TensorFlow Lite models
│   └── movenet_singlepose_lightning.tflite
├── images/                       # Application images
├── database_schema.sql           # Database setup
├── create_admin_user.sql         # Admin user creation
├── compile_and_run_gtkmm.sh      # Build script
└── CMakeLists.txt               # CMake configuration
```

## 🔧 Technical Stack

- **Frontend**: GTKmm 4 (C++)
- **Computer Vision**: OpenCV 4
- **AI/ML**: TensorFlow Lite (MoveNet)
- **Database**: PostgreSQL 14
- **Authentication**: JWT tokens with OpenSSL
- **Build System**: CMake + Shell script
- **Language**: C++17

## 🐛 Troubleshooting

### Common Issues

1. **Camera Access Denied**
   - Ensure camera permissions are granted
   - Check if another application is using the camera

2. **Database Connection Failed**
   - Verify PostgreSQL is running: `brew services start postgresql@14`
   - Check database credentials in `src/main.cc`
   - Ensure database `fitnessdb` exists

3. **Compilation Errors**
   - Update Homebrew packages: `brew update && brew upgrade`
   - Check OpenSSL installation: `brew install openssl@3`
   - Verify GTKmm4 installation: `brew install gtkmm4`

4. **Model Loading Issues**
   - Ensure `movenet_singlepose_lightning.tflite` is in the `models/` directory
   - Check file permissions

### Performance Optimization
- **GPU Acceleration**: TensorFlow Lite supports GPU delegates
- **Memory Management**: Automatic cleanup of pose detection windows
- **Database Indexing**: Optimized queries with proper indexes

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## 📄 License

MIT License - see LICENSE file for details

## 🙏 Acknowledgments

- **TensorFlow Lite**: For the MoveNet pose detection model
- **GTKmm**: For the modern GUI framework
- **OpenCV**: For computer vision capabilities
- **PostgreSQL**: For robust database management

## 📞 Support

For issues and questions:
1. Check the troubleshooting section
2. Review the database schema
3. Verify all dependencies are installed
4. Create an issue on GitHub

---

**Note**: This application is designed for educational and fitness purposes. Always consult with a fitness professional before starting any exercise program. 