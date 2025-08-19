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

# Compile the application
clang++ -std=c++17 \
    -I$POSTGRES_INCLUDE \
    $TFLITE_INCLUDE \
    $(pkg-config gtkmm-4.0 --cflags) \
    $(pkg-config opencv4 --cflags) \
    -I/opt/homebrew/opt/openssl@3/include \
    src/main.cc \
    src/VirtualFitnessCoachWindow.cc \
    src/DatabaseManager.cc \
    src/ExerciseDetailWindow.cc \
    src/PoseDetectionWindow.cc \
    src/AuthManager.cc \
    src/LoginWindow.cc \
    src/RegisterWindow.cc \
    src/MyPageWindow.cc \
    src/PasswordChangeWindow.cc \
    src/User.cc \
    -o VirtualExerciseCoach \
    $(pkg-config gtkmm-4.0 --libs) \
    $(pkg-config opencv4 --libs) \
    -L/opt/homebrew/opt/openssl@3/lib -lssl -lcrypto \
    -L$POSTGRES_LIB -lpq \
    -L$TFLITE_BUILD_DIR/pthreadpool -lpthreadpool \
    -L$TFLITE_DEPS_DIR/abseil-cpp-build/absl/base -labsl_base -labsl_log_severity -labsl_raw_logging_internal -labsl_throw_delegate -labsl_malloc_internal -labsl_scoped_set_env -labsl_strerror -labsl_spinlock_wait \
    -L$TFLITE_DEPS_DIR/abseil-cpp-build/absl/strings -labsl_strings -labsl_strings_internal -labsl_str_format_internal -labsl_cord -labsl_cord_internal -labsl_cordz_info -labsl_cordz_handle -labsl_cordz_functions -labsl_cordz_sample_token -labsl_string_view \
    $TFLITE_DEPS_DIR/abseil-cpp-build/absl/strings/libabsl_strings.a \
    $TFLITE_DEPS_DIR/abseil-cpp-build/absl/base/libabsl_raw_logging_internal.a \
    -L$TFLITE_DEPS_DIR/abseil-cpp-build/absl/types -labsl_bad_optional_access -labsl_bad_any_cast_impl -labsl_bad_variant_access \
    -L$TFLITE_DEPS_DIR/abseil-cpp-build/absl/container -labsl_raw_hash_set -labsl_hashtablez_sampler \
    -L$TFLITE_DEPS_DIR/abseil-cpp-build/absl/hash -labsl_hash -labsl_city -labsl_low_level_hash \
    -L$TFLITE_DEPS_DIR/abseil-cpp-build/absl/numeric -labsl_int128 \
    -L$TFLITE_DEPS_DIR/abseil-cpp-build/absl/status -labsl_status -labsl_statusor \
    -L$TFLITE_DEPS_DIR/abseil-cpp-build/absl/time -labsl_time -labsl_time_zone -labsl_civil_time \
    -L$TFLITE_DEPS_DIR/abseil-cpp-build/absl/synchronization -labsl_synchronization -labsl_graphcycles_internal -labsl_kernel_timeout_internal \
    -L$TFLITE_DEPS_DIR/abseil-cpp-build/absl/random -labsl_random_distributions -labsl_random_seed_sequences -labsl_random_internal_platform -labsl_random_internal_pool_urbg -labsl_random_internal_seed_material -labsl_random_internal_randen_slow -labsl_random_internal_randen_hwaes_impl -labsl_random_internal_randen_hwaes -labsl_random_internal_randen -labsl_random_internal_distribution_test_util -labsl_random_seed_gen_exception \
    -L$TFLITE_DEPS_DIR/abseil-cpp-build/absl/debugging -labsl_debugging_internal -labsl_stacktrace -labsl_symbolize -labsl_demangle_internal -labsl_leak_check -labsl_examine_stack -labsl_failure_signal_handler \
    -L$TFLITE_DEPS_DIR/abseil-cpp-build/absl/crc -labsl_crc32c -labsl_crc_internal -labsl_crc_cpu_detect -labsl_crc_cord_state \
    -L$TFLITE_DEPS_DIR/abseil-cpp-build/absl/profiling -labsl_periodic_sampler -labsl_exponential_biased \
    -L$TFLITE_DEPS_DIR/abseil-cpp-build/absl/log -labsl_log_initialize -labsl_log_globals -labsl_log_internal_globals -labsl_log_internal_format -labsl_log_internal_check_op -labsl_log_internal_message -labsl_log_internal_conditions -labsl_log_internal_nullguard -labsl_log_internal_log_sink_set -labsl_log_internal_proto -labsl_log_entry -labsl_log_sink -labsl_log_flags -labsl_die_if_null \
    -L$TFLITE_DEPS_DIR/abseil-cpp-build/absl/flags -labsl_flags -labsl_flags_commandlineflag -labsl_flags_commandlineflag_internal -labsl_flags_config -labsl_flags_internal -labsl_flags_marshalling -labsl_flags_parse -labsl_flags_program_name -labsl_flags_reflection -labsl_flags_usage -labsl_flags_usage_internal -labsl_flags_private_handle_accessor \
    -L$TFLITE_DEPS_DIR/protobuf-build -lprotobuf -lprotobuf-lite \
    -L$TFLITE_DEPS_DIR/flatbuffers-build -lflatbuffers \
    -L$TFLITE_DEPS_DIR/ruy-build/ruy -lruy_system_aligned_alloc -lruy_apply_multiplier -lruy_kernel_arm -lruy_allocator -lruy_pack_avx -lruy_trmul -lruy_kernel_avx2_fma -lruy_pack_arm -lruy_have_built_path_for_avx -lruy_kernel_avx512 -lruy_have_built_path_for_avx512 -lruy_kernel_avx -lruy_tune -lruy_denormal -lruy_have_built_path_for_avx2_fma -lruy_pack_avx512 -lruy_blocking_counter -lruy_prepare_packed_matrices -lruy_ctx -lruy_thread_pool -lruy_profiler_profiler -lruy_profiler_instrumentation -lruy_prepacked_cache -lruy_context -lruy_block_map -lruy_cpuinfo -lruy_frontend -lruy_pack_avx2_fma -lruy_wait -lruy_context_get_ctx \
    -L$TFLITE_DEPS_DIR/gemmlowp-build -leight_bit_int_gemm \
    -L$TFLITE_DEPS_DIR/cpuinfo-build -lcpuinfo -lcpuinfo_internals \
    -L$TFLITE_DEPS_DIR/farmhash-build -lfarmhash \
    -L$TFLITE_DEPS_DIR/fft2d-build -lfft2d_alloc -lfft2d_fftsg -lfft2d_fftsg3d -lfft2d_fftsg2d -lfft2d_shrtdct \
    $TFLITE_LIB \
    -ldl -lpthread

# Copy the model to the current directory for runtime
cp models/movenet_singlepose_lightning.tflite .

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful. Running the application..."
    ./VirtualExerciseCoach
else
    echo "Compilation failed."
    exit 1
fi
