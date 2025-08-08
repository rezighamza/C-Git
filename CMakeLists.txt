# Minimum version of CMake required
cmake_minimum_required(VERSION 3.10)

# Define the project name
project(c-git VERSION 1.0)

# Set the C++ standard we want to use (C++17 is a good choice)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Find required libraries ---
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

# --- THIS IS THE MODIFIED SECTION ---
# Add an executable target and list its source files explicitly.
# This is more robust than file(GLOB).
add_executable(c-git
    src/main.cpp
    src/commands.cpp
    src/objects.cpp
    src/utils.cpp
)
# -----------------------------------

# --- Link libraries to our executable ---
target_link_libraries(c-git
    PRIVATE
        OpenSSL::Crypto
        ZLIB::ZLIB
)