# This is the main CMake configuration file
# Make sure CMake is synced whenever you make the changes
cmake_minimum_required(VERSION 3.24)
project(my-mini-compiler C)

set(CMAKE_C_STANDARD 11)

# Add include directory (this will be needed to add your tokens to your lexer)
include_directories(${PROJECT_SOURCE_DIR}/phase3-w25/include)

# Add executables when needed: Make sure you specify the path to your .c or .h file
add_executable(my-mini-compiler
        phase3-w25/include/tokens.h
        phase3-w25/include/lexer.h
        phase3-w25/include/parser.h
        phase3-w25/include/semantic.h
        phase3-w25/src/parser/parser.c
        phase3-w25/src/lexer/lexer.c
        phase3-w25/src/semantic/semantic.c)