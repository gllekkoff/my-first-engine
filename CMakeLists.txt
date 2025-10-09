cmake_minimum_required(VERSION 3.16)
project(my-first-engine)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} main.cpp
        glad.c
        cameraClass.h
        callbacks.h)

find_package(glfw3 3.3 REQUIRED)
find_package(glm CONFIG REQUIRED)

set(SHADER_DIR ${CMAKE_SOURCE_DIR}/shaders)
#file(COPY ${SHADER_DIR} DESTINATION ${CMAKE_BINARY_DIR})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME}
        glfw
        GLU
        GL
        Xrandr
        Xi
        pthread
        dl
        m
        glm::glm
)
