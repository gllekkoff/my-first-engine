set(TARGET HomeWork1)
set(HW hw1)
add_executable(${TARGET} src/main.cpp)

target_include_directories(${TARGET} PUBLIC
        ${CMAKE_SOURCE_DIR}/${HW}/include
        ${stb_SOURCE_DIR}
        ${khr_SOURCE_DIR}/api
)

target_link_libraries(${TARGET} PUBLIC
        OpenGL::GL
        glfw
        glm::glm
        assimp
        glad
)

file(COPY ${CMAKE_SOURCE_DIR}/${HW}/shaders DESTINATION ${CMAKE_BINARY_DIR}/${HW})
