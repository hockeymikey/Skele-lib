cmake_minimum_required(VERSION 3.6 FATAL_ERROR)
project(AudioProcessor)

SET(BUILD_SHARED_LIBS OFF)

if(UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

# Download and unpack googletest at configure time
#configure_file(CMakeLists.txt.gtest googletest-download/CMakeLists.txt)
#execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
#  RESULT_VARIABLE result
#  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
#if(result)
#  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
#endif()
#execute_process(COMMAND ${CMAKE_COMMAND} --build .
#  RESULT_VARIABLE result
#  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
#if(result)
#  message(FATAL_ERROR "Build step for googletest failed: ${result}")
#endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
#set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
#add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
#                 ${CMAKE_BINARY_DIR}/googletest-build)
# Setup testing
#enable_testing()



# Google Test checked out from https://github.com/google/googletest
add_subdirectory(libs/googletest)
# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
enable_testing()

# LAME-CMAKE checked out from https://github.com/Iunusov/libmp3lame-CMAKE
add_subdirectory(libs/libmp3lame)

add_subdirectory(src/main/cpp)
add_subdirectory(src/test/cpp)

