cmake_minimum_required(VERSION 3.6 FATAL_ERROR)
set(target audioprocessor)
set(libap_SRCS
    cpp/audio_processor.hpp
    cpp/audio_processor.cpp
    cpp/stream_writer.hpp
    cpp/stream_writer.cpp
    cpp/signal_processor.hpp
    cpp/mp3_compressor.hpp
    cpp/mp3_compressor.cpp
    cpp/audio_buffer.hpp
    cpp/file.hpp
    cpp/system_file.hpp
    cpp/system_file.cpp
    cpp/pcm_processor.hpp
    cpp/pcm_processor.cpp
    cpp/circular_queue.hpp
)


# Declare the library
add_library(${target} STATIC ${libap_SRCS})


# Specify here the include directories exported
# by this library
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${target} mp3lame)
