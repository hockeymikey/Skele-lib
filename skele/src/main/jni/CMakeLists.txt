cmake_minimum_required(VERSION 3.6 FATAL_ERROR)

find_package(Java REQUIRED)

#Path can be messed up. Next few lines are a work around so it can compile
set(JAVA_AWT_LIBRARY NotNeeded)
set(JAVA_JVM_LIBRARY NotNeeded)
set(JAVA_INCLUDE_PATH2 NotNeeded)
set(JAVA_AWT_INCLUDE_PATH NotNeeded)

find_package(JNI REQUIRED)

project(cogi_jni_skele)
set(target cogi_jni_skele)


SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set(jni_SRCS
        skele_wrapper.cpp
        skele_wrapper.hpp)

add_library(${target} SHARED ${jni_SRCS})

target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${target} cogi_skele log)