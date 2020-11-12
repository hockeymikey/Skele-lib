cmake_minimum_required(VERSION 3.6 FATAL_ERROR)
project(cogi_audioprocessor)
set(target cogi_audioprocessor)
set(libap_SRCS
    audio_processor.hpp
    audio_processor.cpp
    stream_writer.hpp
    stream_writer.cpp
    signal_processor.hpp
    mp3_compressor.hpp
    mp3_compressor.cpp
    audio_buffer.hpp
    file.hpp
    system_file.hpp
    system_file.cpp
    pcm_processor.hpp
    pcm_processor.cpp
    circular_queue.hpp
    stream_writer_observer.cpp
        audio_processor_bridge.cpp
)


# Declare the library
add_library(${target} STATIC ${libap_SRCS})


# Specify here the include directories exported
# by this library
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${target} mp3lame)
