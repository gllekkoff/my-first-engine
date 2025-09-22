cmake_minimum_required(VERSION 3.16)
project(my-first-engine)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} main.cpp glad.c)

find_package(glfw3 3.3 REQUIRED)

target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME}
        glfw
        GLU
        GL
        Xrandr
        Xi
        pthread
        dl
        m
)
