set(TARGET HomeWork3)
set(HW hw3)
add_executable(${TARGET}
        src/main.cpp
        src/shader.cpp
        src/lights.cpp
        src/cameraClass.cpp
        src/model.cpp
        src/performanceLog.cpp
        src/timeClass.cpp
)

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
file(COPY ${CMAKE_SOURCE_DIR}/${HW}/textures DESTINATION ${CMAKE_BINARY_DIR}/${HW})
