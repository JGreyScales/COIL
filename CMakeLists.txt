cmake_minimum_required(VERSION 3.10)
project(COIL_Project)

# Use C++17 or higher (standard for Crow and modern Asio)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Find Boost and its required components
# system: error handling, thread: concurrency, regex: Crow route parsing
find_package(Boost REQUIRED COMPONENTS system thread regex)
find_package(Threads REQUIRED)

# 2. Include directories
include_directories(COIL)
include_directories(${Boost_INCLUDE_DIRS})

# Define all source files
set(SOURCE_FILES
    COIL/COIL.cpp
    COIL/AckBodyDef.cpp
    COIL/BodyInterface.cpp
    COIL/DriveBodyDef.cpp
    COIL/HeaderDef.cpp
    COIL/MySocket.cpp
    COIL/PktDef.cpp
    COIL/Robot.cpp
    COIL/TelemetryBodyDef.cpp
    COIL/TurnBodyDef.cpp
)

# Create the executable
add_executable(COIL_App.out ${SOURCE_FILES})

# 3. Link libraries
# Boost_LIBRARIES includes system, thread, and regex as requested above
target_link_libraries(COIL_App.out 
    PRIVATE 
    Threads::Threads
    ${Boost_LIBRARIES}
)
