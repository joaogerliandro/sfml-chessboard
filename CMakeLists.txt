cmake_minimum_required(VERSION 3.10)
project(chess-board VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_EXTENSIONS ON) 
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_executable(
    ${PROJECT_NAME}
    src/main.cpp
)

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_link_libraries(
    ${PROJECT_NAME} 
    sfml-graphics
    sfml-window
    sfml-system
    GL
)
