cmake_minimum_required(VERSION 3.28)

project(lolol LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

find_package(spdlog CONFIG REQUIRED)

add_executable(lolol 
    src/main.cpp
    src/videodevice.cpp
    src/devicemanager.cpp
    src/frame.cpp
    src/utils/framelimiter.cpp
)

target_link_libraries(lolol PRIVATE spdlog::spdlog)

# Testing
enable_testing()
add_subdirectory(tests)
