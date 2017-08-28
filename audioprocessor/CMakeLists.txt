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
#execute_process(COMMAND ${CMAKE_COMMAND}  -DCMAKE_MAKE_PROGRAM="C:\\Users\\andrew.jacobson\\AppData\\Local\\Android\\sdk\\cmake\\3.6.4111459\\bin\\ninja.exe" "-GAndroid Gradle - Ninja" . "-DANDROID_ABI=armeabi -DANDROID_NDK=C:\\Users\\andrew.jacobson\\AppData\\Local\\Android\\sdk\\ndk-bundle -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=C:\\Users\\andrew.jacobson\\Documents\\Projects\\cogi-audio-processor\\audioprocessor\\build\\intermediates\\cmake\\debug\\obj\\armeabi -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=C:\\Users\\andrew.jacobson\\AppData\\Local\\Android\\sdk\\ndk-bundle\\build\\cmake\\android.toolchain.cmake -DANDROID_PLATFORM=android-14 -DCMAKE_CXX_FLAGS=-std=c++11 -frtti -fexceptions"
#execute_process(COMMAND ${CMAKE_COMMAND} "-DCMAKE_CXX_FLAGS=-std=c++11" -frtti -fexceptions -DCMAKE_MAKE_PROGRAM="C:\\Users\\andrew.jacobson\\AppData\\Local\\Android\\sdk\\cmake\\3.6.4111459\\bin\\ninja.exe" "-GAndroid Gradle - Ninja" .
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



add_subdirectory(src/main)
add_subdirectory(src/test)

