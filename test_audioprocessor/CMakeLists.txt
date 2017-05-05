
SET(target test_audioprocessor)
ADD_EXECUTABLE(${target} audio_processor_test.cpp stream_writer_test.cpp  compressed_stream_writer_test.cpp)


TARGET_LINK_LIBRARIES(${target} audioprocessor gtest_main gmock)

#file(COPY test_wave.wav DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
