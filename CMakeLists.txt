cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(FEM                 # Project name
        VERSION 1.0         # Version programm
        LANGUAGES CXX
)

set(EXECUTABLE_OUTPUT_PATH "bin")       # Directory output file

set(SOURCES
    src/main.cpp
)

set(HEADERS
    include/argparse/argparse.hpp
    include/cross-platform/filesystem.hpp
    include/indicators/indicators.hpp
    include/LOS/Data.hpp
    include/LOS/LOS.hpp
    include/LOS/LOS_Function.hpp
    include/nlohmann/json.hpp
    include/timer/cxxtimer.hpp
    include/utils/argv.hpp
    include/utils/friendly.hpp
    include/utils/lightweight.hpp
    include/utils/overload.hpp
    include/FEM.hpp
    include/Function.hpp
    include/Logger.hpp
    include/Union.hpp
)

add_executable(FEM ${SOURCES} ${HEADERS})

set_target_properties(
        FEM PROPERTIES
        CXX_STANDARD          20
        CXX_STANDARD_REQUIRED ON
)

target_include_directories(
    FEM PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
