cmake_minimum_required(VERSION 3.10)
project(chess-board VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_EXTENSIONS ON) 
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

IF(WIN32)
    include_directories(
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/lib/SFML/include
        ${CMAKE_SOURCE_DIR}/lib/GLM
    )

    link_directories(
        ${CMAKE_SOURCE_DIR}/lib/SFML/lib
    )
ELSE()
ENDIF()

add_executable(
    ${PROJECT_NAME}
    src/main.cpp
    include/gl_init.hpp
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
)

IF(WIN32)
    target_link_libraries(
        ${PROJECT_NAME}
        opengl32
    )
ELSE()
    target_link_libraries(
        ${PROJECT_NAME}
        GL
    )
ENDIF()