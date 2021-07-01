cmake_minimum_required(VERSION 3.6 FATAL_ERROR)
project(Skele)

SET(BUILD_SHARED_LIBS ON)

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DANDROID_STL=c++static")
#add_compile_options(-Wno-sign-conversion)

# Google Test checked out from https://github.com/google/googletest
# Prevent overriding the parent project's compiler/linker
# settings on Windows
#set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
#enable_testing()

# LAME-CMAKE checked out from https://github.com/Iunusov/libmp3lame-CMAKE

add_subdirectory(src/main/cpp)
add_subdirectory(src/main/jni)