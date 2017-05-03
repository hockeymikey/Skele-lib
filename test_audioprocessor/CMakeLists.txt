
ADD_EXECUTABLE(test_audioprocessor audio_processor_test.cpp audio_processor_test.hpp stream_writer_test.cpp stream_writer_test.hpp)

TARGET_LINK_LIBRARIES(test_audioprocessor
    audioprocessor
    gtest_main
    gmock
    ${Boost_LIBRARIES}
)

#add_test(NAME testfoo COMMAND testfoo)
