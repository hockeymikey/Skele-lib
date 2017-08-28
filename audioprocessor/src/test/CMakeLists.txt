
SET(target test_audioprocessor)
ADD_EXECUTABLE(${target}
    cpp/mp3_compressor_mock.hpp
    cpp/audio_processor_test.hpp
    cpp/audio_processor_test.cpp
    cpp/stream_writer_test.hpp
    cpp/stream_writer_test.cpp
    cpp/mp3_compressor_test.hpp
    cpp/mp3_compressor_test.cpp
    cpp/audio_buffer_test.hpp
    cpp/audio_buffer_test.cpp
    cpp/circular_queue_test.hpp
    cpp/circular_queue_test.cpp
    cpp/test_stream_writer_observer.hpp)


TARGET_LINK_LIBRARIES(${target} audioprocessor gtest_main gmock)

#file(COPY test_wave.wav DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
