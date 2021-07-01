cmake_minimum_required(VERSION 3.6 FATAL_ERROR)
project(cogi_skele)
set(target cogi_skele)
set(libap_SRCS
    skele.hpp
    skele.cpp
)


# Declare the library
add_library(${target} STATIC ${libap_SRCS})


# Specify here the include directories exported
# by this library
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${target})