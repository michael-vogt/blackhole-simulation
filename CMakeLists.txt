cmake_minimum_required(VERSION 3.20)

project(BlackHoleSimulation)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME}
        src/main.cpp
)

find_package(OpenGL REQUIRED)

set(OPENGL_DEPS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../opengl")

target_include_directories(${PROJECT_NAME} PRIVATE
    "${OPENGL_DEPS_DIR}/glfw/include"
    "${OPENGL_DEPS_DIR}/glew/include"
    "${OPENGL_DEPS_DIR}/glm"
)

target_link_directories(${PROJECT_NAME} PRIVATE
        "${OPENGL_DEPS_DIR}/glfw/lib-mingw-w64"
        "${OPENGL_DEPS_DIR}/glew/lib/Release/x64"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        glfw3
        glew32
        OpenGL::GL
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${OPENGL_DEPS_DIR}/glfw/lib-mingw-w64/glfw3.dll"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${OPENGL_DEPS_DIR}/glew/bin/Release/x64/glew32.dll"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
)