cmake_minimum_required(VERSION 3.16)
project(VirtualExerciseCoach CXX)

set(CMAKE_CXX_STANDARD 17)

# Find dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED IMPORTED_TARGET gtkmm-4.0)
pkg_check_modules(OpenCV REQUIRED IMPORTED_TARGET opencv4)

# Find OpenSSL for JWT functionality
find_package(OpenSSL REQUIRED)

include_directories(
    ${GTKMM_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/tensorflow-2.18.1/tensorflow/lite
    /opt/homebrew/opt/postgresql@14/include/postgresql@14
)

link_directories(
    /opt/homebrew/opt/postgresql@14/lib/postgresql@14
    tensorflow-2.18.1/tensorflow/lite/build
    tensorflow-2.18.1/tensorflow/lite/build/pthreadpool
    tensorflow-2.18.1/tensorflow/lite/build/_deps/abseil-cpp-build/absl/base
    tensorflow-2.18.1/tensorflow/lite/build/_deps/abseil-cpp-build/absl/strings
    tensorflow-2.18.1/tensorflow/lite/build/_deps/abseil-cpp-build/absl/types
    tensorflow-2.18.1/tensorflow/lite/build/_deps/abseil-cpp-build/absl/container
    tensorflow-2.18.1/tensorflow/lite/build/_deps/abseil-cpp-build/absl/hash
    tensorflow-2.18.1/tensorflow/lite/build/_deps/abseil-cpp-build/absl/numeric
    tensorflow-2.18.1/tensorflow/lite/build/_deps/abseil-cpp-build/absl/status
    tensorflow-2.18.1/tensorflow/lite/build/_deps/abseil-cpp-build/absl/time
    tensorflow-2.18.1/tensorflow/lite/build/_deps/abseil-cpp-build/absl/synchronization
    tensorflow-2.18.1/tensorflow/lite/build/_deps/abseil-cpp-build/absl/random
    tensorflow-2.18.1/tensorflow/lite/build/_deps/abseil-cpp-build/absl/debugging
    tensorflow-2.18.1/tensorflow/lite/build/_deps/abseil-cpp-build/absl/crc
    tensorflow-2.18.1/tensorflow/lite/build/_deps/abseil-cpp-build/absl/profiling
    tensorflow-2.18.1/tensorflow/lite/build/_deps/abseil-cpp-build/absl/log
    tensorflow-2.18.1/tensorflow/lite/build/_deps/abseil-cpp-build/absl/flags
    tensorflow-2.18.1/tensorflow/lite/build/_deps/protobuf-build
    tensorflow-2.18.1/tensorflow/lite/build/_deps/flatbuffers-build
    tensorflow-2.18.1/tensorflow/lite/build/_deps/ruy-build/ruy
    tensorflow-2.18.1/tensorflow/lite/build/_deps/gemmlowp-build
    tensorflow-2.18.1/tensorflow/lite/build/_deps/cpuinfo-build
    tensorflow-2.18.1/tensorflow/lite/build/_deps/farmhash-build
    tensorflow-2.18.1/tensorflow/lite/build/_deps/fft2d-build
)

file(GLOB SRC_FILES src/*.cc)

add_executable(VirtualExerciseCoach ${SRC_FILES})

add_custom_command(TARGET VirtualExerciseCoach POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/models/movenet_singlepose_lightning.tflite
        $<TARGET_FILE_DIR:VirtualExerciseCoach>
)

target_link_libraries(VirtualExerciseCoach
    PkgConfig::GTKMM
    PkgConfig::OpenCV
    OpenSSL::SSL OpenSSL::Crypto
    pq
    tensorflow-lite
    pthreadpool
    absl_base absl_log_severity absl_raw_logging_internal absl_throw_delegate absl_malloc_internal absl_scoped_set_env absl_strerror absl_spinlock_wait
    absl_strings absl_strings_internal absl_str_format_internal absl_cord absl_cord_internal absl_cordz_info absl_cordz_handle absl_cordz_functions absl_cordz_sample_token absl_string_view
    absl_bad_optional_access absl_bad_any_cast_impl absl_bad_variant_access
    absl_raw_hash_set absl_hashtablez_sampler
    absl_hash absl_city absl_low_level_hash
    absl_int128
    absl_status absl_statusor
    absl_time absl_time_zone absl_civil_time
    absl_synchronization absl_graphcycles_internal absl_kernel_timeout_internal
    absl_random_distributions absl_random_seed_sequences absl_random_internal_platform absl_random_internal_pool_urbg absl_random_internal_seed_material absl_random_internal_randen_slow absl_random_internal_randen_hwaes_impl absl_random_internal_randen_hwaes absl_random_internal_randen absl_random_internal_distribution_test_util absl_random_seed_gen_exception
    absl_debugging_internal absl_stacktrace absl_symbolize absl_demangle_internal absl_leak_check absl_examine_stack absl_failure_signal_handler
    absl_crc32c absl_crc_internal absl_crc_cpu_detect absl_crc_cord_state
    absl_periodic_sampler absl_exponential_biased
    absl_log_initialize absl_log_globals absl_log_internal_globals absl_log_internal_format absl_log_internal_check_op absl_log_internal_message absl_log_internal_conditions absl_log_internal_nullguard absl_log_internal_log_sink_set absl_log_internal_proto absl_log_entry absl_log_sink absl_log_flags absl_die_if_null
    absl_flags absl_flags_commandlineflag absl_flags_commandlineflag_internal absl_flags_config absl_flags_internal absl_flags_marshalling absl_flags_parse absl_flags_program_name absl_flags_reflection absl_flags_usage absl_flags_usage_internal absl_flags_private_handle_accessor
    protobuf protobuf-lite
    flatbuffers
    ruy_system_aligned_alloc ruy_apply_multiplier ruy_kernel_arm ruy_allocator ruy_pack_avx ruy_trmul ruy_kernel_avx2_fma ruy_pack_arm ruy_have_built_path_for_avx ruy_kernel_avx512 ruy_have_built_path_for_avx512 ruy_kernel_avx ruy_tune ruy_denormal ruy_have_built_path_for_avx2_fma ruy_pack_avx512 ruy_blocking_counter ruy_prepare_packed_matrices ruy_ctx ruy_thread_pool ruy_profiler_profiler ruy_profiler_instrumentation ruy_prepacked_cache ruy_context ruy_block_map ruy_cpuinfo ruy_frontend ruy_pack_avx2_fma ruy_wait ruy_context_get_ctx
    eight_bit_int_gemm
    cpuinfo cpuinfo_internals
    farmhash
    fft2d_alloc fft2d_fftsg fft2d_fftsg3d fft2d_fftsg2d fft2d_shrtdct
    tensorflow-lite
    dl pthread
) 