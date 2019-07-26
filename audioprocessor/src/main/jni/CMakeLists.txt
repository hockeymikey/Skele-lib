cmake_minimum_required(VERSION 3.6 FATAL_ERROR)

find_package(Java REQUIRED)
set(JAVA_AWT_LIBRARY NotNeeded)
set(JAVA_JVM_LIBRARY NotNeeded)
set(JAVA_INCLUDE_PATH2 NotNeeded)
set(JAVA_AWT_INCLUDE_PATH NotNeeded)
find_package(JNI REQUIRED)

project(cogi_jni_audioprocessor)
set(target cogi_jni_audioprocessor)


SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


set(libap_SRCS
        wrapper.cpp
        wrapper.hpp
        )

add_library(${target} SHARED ${libap_SRCS})

target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${target} cogi_audioprocessor)