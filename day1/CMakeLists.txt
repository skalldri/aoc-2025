cmake_minimum_required(VERSION 3.15)

# Set the project name and programming language
project(MySimpleProject LANGUAGES CXX)

# Require C++17 (or C++20 / C++23 depending on your needs)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define the executable name and its source files
add_executable(day1 main.cpp)