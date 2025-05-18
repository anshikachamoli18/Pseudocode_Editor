cmake_minimum_required(VERSION 3.10)
project(pseudocode_compiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Output in build/Debug/ with name pseudocode_compiler.exe
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include_directories(include)

set(SOURCES
    src/main.cpp
    src/lexer.cpp
    src/parser.cpp
    src/semantic_analyzer.cpp
    src/ir_generator.cpp
    src/ir_optimizer.cpp
    src/ir_interpreter.cpp
)

add_executable(pseudocode_compiler ${SOURCES})
