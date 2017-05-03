set(libap_SRCS
    audio_processor.hpp
    audio_processor.cpp
    audio_description.hpp
    audio_description.cpp
    stream_writer.hpp
    stream_writer.cpp
    compressed_stream_writer.hpp
    compressed_stream_writer.cpp
)

# Declare the library
add_library(audioprocessor STATIC
    ${libap_SRCS}
)

# Specify here the include directories exported
# by this library
target_include_directories(audioprocessor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR} ${Boost_LIBRARIES}
)
