# Set the minimum required version of CMake for this project
cmake_minimum_required(VERSION 3.5)

# Define the project name and the language it uses
project(keygen C)

# Add an executable to the project using the specified source file
add_executable(keygen src/main.c src/info.c)