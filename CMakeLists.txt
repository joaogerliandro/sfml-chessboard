cmake_minimum_required(VERSION 3.16)
project(sfml-chessboard VERSION 0.1.0 LANGUAGES C CXX)

message(STATUS "Project: ${PROJECT_NAME} (version ${CMAKE_PROJECT_VERSION})")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "CMake Generator: ${CMAKE_GENERATOR}")

include(FetchContent)

set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_EXTENSIONS ON) 
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(BUILD_SHARED_LIBS FALSE)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/)

FetchContent_Declare(
    sfml
    GIT_REPOSITORY https://github.com/SFML/SFML
    GIT_TAG master
)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm
    GIT_TAG master
)

FetchContent_MakeAvailable(SFML)
FetchContent_MakeAvailable(glm)

find_package(OpenGL REQUIRED)

file(GLOB_RECURSE SRCS RELATIVE ${PROJECT_SOURCE_DIR} "src/*.cpp")

add_executable(
    ${PROJECT_NAME}
    ${SRCS}
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    ${OPENGL_LIBRARIES}
    sfml-system 
    sfml-graphics
    sfml-window
    sfml-audio 
    glm::glm 
)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)