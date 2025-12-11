#ifndef MY_FIRST_ENGINE_MAIN_H
#define MY_FIRST_ENGINE_MAIN_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

#define PERF false
#define INSTANCING true
#define STEP_2_MODEL false

constexpr unsigned int SCR_WIDTH = 1280;
constexpr unsigned int SCR_HEIGHT = 720;
constexpr unsigned int amount = 10000;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void mouse_button_callback(GLFWwindow *window, int button, int action, int mode);

void processInput(GLFWwindow *window, float deltaTime);

unsigned int TextureFromFile(const char *path, const std::string &directory);

void loadCubesStep3(const class Model &cube, glm::mat4 modelMatrix[amount]);

#endif
