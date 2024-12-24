cmake_minimum_required(VERSION 3.12)
project(browser)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(--std=c++20)

add_library(url url.cpp)
target_include_directories(url PUBLIC "${PROJECT_SOURCE_DIR}")

add_executable(browser browser.cpp)
target_link_libraries(browser url)
