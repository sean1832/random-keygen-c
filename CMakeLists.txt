# Set the minimum required version of CMake for this project
cmake_minimum_required(VERSION 3.5)

# Define the project name and the language it uses
project(randkey C)

# Add an executable to the project using the specified source file
add_executable(randkey src/main.c src/info.c)

# set the C standard to C99
set_property(TARGET randkey PROPERTY C_STANDARD 99)
