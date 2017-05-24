cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(AudioProcessor)

SET(BUILD_SHARED_LIBS OFF)

if(UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.gtest googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
                 ${CMAKE_BINARY_DIR}/googletest-build)
# Setup testing
enable_testing()



#download and add lame
configure_file(CMakeLists.txt.lame lame-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/lame-download )
if(result)
  message(FATAL_ERROR "CMake step for lame failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/lame-download )
if(result)
  message(FATAL_ERROR "Build step for lame failed: ${result}")
endif()


# Add lame directly to our build. This defines
# the lame targets.
add_subdirectory(${CMAKE_BINARY_DIR}/lame-src
                 ${CMAKE_BINARY_DIR}/lame-build)



add_subdirectory(audioprocessor)
add_subdirectory(test_audioprocessor)

