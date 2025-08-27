#!/bin/bash

# Set PKG_CONFIG_PATH for Homebrew-installed libraries
export PKG_CONFIG_PATH="/opt/homebrew/lib/pkgconfig:/opt/homebrew/opt/opencv/lib/pkgconfig:$PKG_CONFIG_PATH"

# PostgreSQL paths
POSTGRES_INCLUDE="/opt/homebrew/opt/postgresql@14/include/postgresql@14"
POSTGRES_LIB="/opt/homebrew/opt/postgresql@14/lib/postgresql@14"

# TensorFlow Lite paths
TFLITE_LIB="tensorflow-2.18.1/tensorflow/lite/build/libtensorflow-lite.a"
TFLITE_INCLUDE="-Itensorflow-2.18.1/tensorflow/lite"

# TensorFlow Lite library paths
TFLITE_BUILD_DIR="tensorflow-2.18.1/tensorflow/lite/build"
TFLITE_DEPS_DIR="$TFLITE_BUILD_DIR/_deps"

# Clean build directory
echo "Cleaning build directory..."
rm -rf build

# Compile the application using CMake
cmake -B build
cmake --build build

# Copy the model to the build directory for runtime
cp models/movenet_singlepose_lightning.tflite build/

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful. Running the application..."
    ./build/VirtualExerciseCoach
else
    echo "Compilation failed."
    exit 1
fi
