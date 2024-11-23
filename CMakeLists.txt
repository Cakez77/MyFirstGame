cmake_minimum_required(VERSION 3.0)

# We are building a C Game
set(CMAKE_C_STANDARD 99)
project(game C)
# set(CMAKE_CXX_STANDARD 20)
# project(game CXX)

# Download Raylib
include(FetchContent)
FetchContent_Declare(raylib GIT_REPOSITORY "https://github.com/raysan5/raylib.git")
FetchContent_MakeAvailable(raylib)

# Include and Link Raylib in the game
add_executable(${PROJECT_NAME} "game.c")
target_link_libraries(${PROJECT_NAME} raylib)
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR})