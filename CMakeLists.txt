cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(AudioProcessor)

set(Boost_USE_STATIC_LIBS ON)
#set(Boost_USE_MULTITHREADED OFF)
#set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost REQUIRED COMPONENTS system filesystem)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
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


#lame 3.99.5
FIND_PATH(LAME_INCLUDE_DIR lame/lame.h)
FIND_LIBRARY(LAME_LIBRARIES NAMES mp3lame)
IF(LAME_INCLUDE_DIR AND LAME_LIBRARIES)
	SET(LAME_FOUND TRUE)
ENDIF(LAME_INCLUDE_DIR AND LAME_LIBRARIES)

IF(LAME_FOUND)
	IF (NOT Lame_FIND_QUIETLY)
		MESSAGE(STATUS "Found lame includes:	${LAME_INCLUDE_DIR}/lame/lame.h")
		MESSAGE(STATUS "Found lame library: ${LAME_LIBRARIES}")
	ENDIF (NOT Lame_FIND_QUIETLY)
ELSE(LAME_FOUND)
	IF (Lame_FIND_REQUIRED)
		MESSAGE(FATAL_ERROR "Could NOT find lame development files")
	ENDIF (Lame_FIND_REQUIRED)
ENDIF(LAME_FOUND)

add_subdirectory(audioprocessor)
add_subdirectory(test_audioprocessor)

