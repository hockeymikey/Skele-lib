
SET(target test_audioprocessor)
ADD_EXECUTABLE(${target} mp3_compressor_mock.hpp audio_processor_test.hpp audio_processor_test.cpp stream_writer_test.hpp stream_writer_test.cpp mp3_compressor_test.hpp  mp3_compressor_test.cpp audio_buffer_test.hpp audio_buffer_test.cpp circular_queue_test.hpp circular_queue_test.cpp)


TARGET_LINK_LIBRARIES(${target} audioprocessor gtest_main gmock)

#file(COPY test_wave.wav DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
