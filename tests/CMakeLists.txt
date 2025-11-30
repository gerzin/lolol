find_package(GTest CONFIG REQUIRED)

# Create a library with the code we want to test (excluding main.cpp)
add_library(lolol_lib
    ../src/videodevice.cpp
    ../src/utils/framelimiter.cpp
)

target_include_directories(lolol_lib PUBLIC ../src)
target_link_libraries(lolol_lib PUBLIC spdlog::spdlog)

# Test executable
add_executable(lolol_tests
    test_framelimiter.cpp
)

target_link_libraries(lolol_tests
    PRIVATE
        lolol_lib
        GTest::gtest
        GTest::gtest_main
)

# Discover tests for CTest
include(GoogleTest)
gtest_discover_tests(lolol_tests)
