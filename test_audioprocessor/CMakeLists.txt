
ADD_EXECUTABLE(test_audioprocessor test_audioprocessor.cpp test_audioprocessor.hpp)

TARGET_LINK_LIBRARIES(test_audioprocessor
    audioprocessor
    gtest_main
    gmock
    ${Boost_LIBRARIES}
)

#add_test(NAME testfoo COMMAND testfoo)
