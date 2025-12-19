set(TARGET HomeWork5)
set(HW hw5)
add_executable(${TARGET}
        src/main.cpp
        src/shader.cpp
        src/lights.cpp
        src/model.cpp
        src/camera.cpp
        src/timeManager.cpp
        src/generateBayerTexture.cpp
        src/dithering.cpp
        src/scene.cpp
        src/renderSettings.cpp
        src/hdrSystem.cpp
        src/pbrMaterial.cpp
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
        ImGui
)

file(COPY ${CMAKE_SOURCE_DIR}/${HW}/shaders DESTINATION ${CMAKE_BINARY_DIR}/${HW})
file(COPY ${CMAKE_SOURCE_DIR}/${HW}/textures DESTINATION ${CMAKE_BINARY_DIR}/${HW})

