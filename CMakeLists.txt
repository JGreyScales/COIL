cmake_minimum_required(VERSION 3.10)
project(COIL_Project)

# 1. General Project Settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 2. Find Dependencies
find_package(Boost REQUIRED COMPONENTS system thread regex)
find_package(Threads REQUIRED)

# 3. Define Source Files
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
    COIL/App.cpp
    COIL/CrowRoutes.cpp
)

# 4. Create the Executable
add_executable(COIL_App.out ${SOURCE_FILES})

# 5. Development vs. Production Logic
option(USE_ASAN "Enable Address Sanitizer" OFF)

if(USE_ASAN)
    message(STATUS "Build Mode: Development (ASAN enabled)")
    target_compile_options(COIL_App.out PRIVATE 
        -fsanitize=address 
        -g 
        -O1 
        -fno-omit-frame-pointer
    )
    target_link_options(COIL_App.out PRIVATE 
        -fsanitize=address
    )
else()
    message(STATUS "Build Mode: Production")
    target_compile_options(COIL_App.out PRIVATE -O3)
endif()

# 6. Include Directories
# Includes COIL for local headers and Boost for external headers
target_include_directories(COIL_App.out PRIVATE COIL ${Boost_INCLUDE_DIRS})

# 7. Link Libraries
target_link_libraries(COIL_App.out 
    PRIVATE 
    Threads::Threads
    ${Boost_LIBRARIES}
)