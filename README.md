[日本語のREADMEはこちら](README_ja.md)

# Virtual Exercise Coach

A modern desktop application for real-time exercise pose detection and coaching, built with C++, GTKmm, OpenCV, and TensorFlow Lite.

## Features
- Real-time pose detection using MoveNet (TensorFlow Lite)
- Visual skeleton overlay and keypoint tracking
- Clean GTK4-based GUI
- Modular codebase for easy extension

## Build Instructions

### Using CMake (Recommended)
```sh
mkdir build
cd build
cmake ..
make
./VirtualExerciseCoach
```

### Using the Shell Script
```sh
./compile_and_run_gtkmm.sh
```

## Requirements
- GTKmm 4
- OpenCV 4
- TensorFlow Lite (prebuilt static libs)
- PostgreSQL client libraries (for database features)
- C++17 compiler (clang++ or g++)

## Model
The MoveNet model file (`movenet_singlepose_lightning.tflite`) is required in the `models/` directory. It will be copied to the build/run directory automatically.

## Usage
- Launch the app and allow camera access.
- The app will display your pose with a skeleton overlay in real time.
- Use the GUI to navigate exercises and features.

## Project Structure
- `src/` — C++ source and header files
- `models/` — TensorFlow Lite model(s)
- `images/` — App images/assets
- `build/` — CMake build directory
- `compile_and_run_gtkmm.sh` — Legacy build script

## License
MIT (or your chosen license) 