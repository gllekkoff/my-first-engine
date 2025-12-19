//
// Created by gllekk on 11/14/25.
//

#ifndef MY_FIRST_ENGINE_MAIN_H
#define MY_FIRST_ENGINE_MAIN_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "shader.h"
#include "camera.h"
#include "model.h"
#include "timeManager.h"
#include "performanceLog.h"
#include "lights.h"
#include <X11/Xlib.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "generateBayerTexture.h"
#include "dithering.h"
#include "scene.h"
#include "renderSettings.h"
#include "hdrSystem.h"
#include "pbrMaterial.h"


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
constexpr unsigned int SCR_WIDTH = 1280;
constexpr unsigned int SCR_HEIGHT = 720;

constexpr unsigned int amount = 10000;

inline void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    extern int currentWidth, currentHeight;
    currentWidth = width;
    currentHeight = height;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

inline void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){};
void mouse_button_callback(GLFWwindow* window, int button, int action, int mode);
void processInput(GLFWwindow *window, float deltaTime);
unsigned int LoadTexture(char const *path, const std::string& directory);
void loadCubesStep3(const Model& cube, glm::mat4 modelMatrix[amount]);
void renderModels(Shader& shader, const Model& myModel);
void renderFloor(Shader& shader, const Mesh& planeMesh);
void setupModelMaterial(Shader& shader);
void setupFloorMaterial(Shader& shader, unsigned int groundTexture);


#endif //MY_FIRST_ENGINE_MAIN_H