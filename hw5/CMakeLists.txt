add_executable(${PROJECT_NAME} main.cpp
        include/cameraClass.h
)

target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${stb_SOURCE_DIR}
        ${khr_SOURCE_DIR}/api
)

target_link_libraries(${PROJECT_NAME} PUBLIC
        OpenGL::GL
        glfw
        glm::glm
        assimp
        glad
)