set(libap_SRCS
    AudioProcessor.hpp
    AudioProcessor.cpp
    AudioDescription.hpp
    AudioDescription.cpp
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
