
ADD_EXECUTABLE(test_audioprocessor AudioProcessorTest.cpp AudioProcessorTest.hpp StreamWriterTest.cpp StreamWriterTest.hpp)

TARGET_LINK_LIBRARIES(test_audioprocessor
    audioprocessor
    gtest_main
    gmock
    ${Boost_LIBRARIES}
)

#add_test(NAME testfoo COMMAND testfoo)
