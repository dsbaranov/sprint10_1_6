cmake_minimum_required(VERSION 3.0.0)
project(sprint10_1_6 VERSION 0.1.0 LANGUAGES C CXX)
add_executable(sprint10_1_6 main.cpp log_duration.h svg.h svg.cpp)
target_compile_options(sprint10_1_6 PRIVATE -Wall -Wextra -Wpedantic -Werror)
