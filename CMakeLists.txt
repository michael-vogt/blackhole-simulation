cmake_minimum_required(VERSION 3.20)

project(BlackHoleSimulation)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(BlackHoleSimulation
    src/main.cpp
)

target_link_libraries(BlackHoleSimulation PRIVATE
    glfw
    GLEW::GLEW
    glm::glm
)