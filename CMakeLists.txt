cmake_minimum_required(VERSION 3.0)

project(FEM                 # Project name
        VERSION 1.0         # Version programm
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)              # Version  -std=c++20
set(CMAKE_CXX_STANDARD_REQUIRED ON)     # REQUIRED -std=c++20
set(EXECUTABLE_OUTPUT_PATH "bin")       # Directory output file

add_executable(
        FEM                 # Output file name
        src/main.cpp
)

include_directories(${PROJECT_SOURCE_DIR}/include)