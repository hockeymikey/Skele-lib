set(target audioprocessor)
set(libap_SRCS
    audio_processor.hpp
    audio_processor.cpp
    audio_description.hpp
    audio_description.cpp
    stream_writer.hpp
    stream_writer.cpp
    compressor.hpp
    mp3_compressor.hpp
    mp3_compressor.cpp
)


# Declare the library
add_library(${target} STATIC ${libap_SRCS})


# Specify here the include directories exported
# by this library
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${target} mp3lame)