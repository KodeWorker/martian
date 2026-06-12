add_executable(martian_tests
    test_policy.cpp
    test_simulation.cpp
)
target_link_libraries(martian_tests PRIVATE colony_core GTest::gtest_main)
target_include_directories(martian_tests PRIVATE ${CMAKE_SOURCE_DIR}/src)

include(GoogleTest)
gtest_discover_tests(martian_tests)
