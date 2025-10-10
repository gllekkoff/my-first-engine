#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "cameraClass.h"
#include <iostream>
#include <fstream>
#include <sstream>

unsigned int createShaderProgram(const std::string& vertexFile, const std::string& fragmentFile);
unsigned int compileShader(unsigned int type, const std::string& source);
std::string readShaderFromFile(const std::string& filename);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);


constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;
bool rotateMouseActive = false;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Hello World", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearDepth(1.0);

    constexpr float vertices[] = {
        // front side
        0.5f, 0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        -0.5f,  -0.5f, 0.5f,
        -0.5f,  0.5f, 0.5f,

        // back side
        0.5f, 0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        -0.5f,  -0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,

        // right side
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f,  -0.5f, -0.5f,
        0.5f,  -0.5f, 0.5f,

        // left side
        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, -0.5f,
        -0.5f,  -0.5f, -0.5f,
        -0.5f,  -0.5f, 0.5f,

        // top
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, 0.5f,


        // bottom
        0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, -0.5f,
        -0.5f,  -0.5f, 0.5f,
        -0.5f,  -0.5f, -0.5f,
    };

    constexpr int indices[] = {
        1, 0, 3, 3, 2, 1,

        4, 7, 6, 4, 5, 6,

        8, 9, 10, 10, 11, 8,

        12, 13, 15, 14, 15, 13,

        17, 18, 19, 16, 17, 19,

        20, 21, 23, 23, 22, 20,
    };

    const std::string vertexShaderFile = "shaders/hw1_cube/vertexShader.glsl";
    const std::string fragmentShaderFileA = "shaders/hw1_cube/fragmentShaderA.glsl";
    const std::string fragmentShaderFileB = "shaders/hw1_cube/fragmentShaderB.glsl";

    const unsigned int shaderProgramA = createShaderProgram(vertexShaderFile, fragmentShaderFileA);
    const unsigned int shaderProgramB = createShaderProgram(vertexShaderFile, fragmentShaderFileB);

    unsigned int VBO, VAO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    const int modelA = glGetUniformLocation(shaderProgramA, "model");
    const int viewA  = glGetUniformLocation(shaderProgramA, "view");
    const int projA  = glGetUniformLocation(shaderProgramA, "projection");
    const int modelB = glGetUniformLocation(shaderProgramB, "model");
    const int viewB  = glGetUniformLocation(shaderProgramB, "view");
    const int projB  = glGetUniformLocation(shaderProgramB, "projection");

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        // rendering commands
        glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.GetViewMatrix();
        auto model = glm::mat4(1.0f);
        glm::mat4 proj  = glm::perspective(glm::radians(camera.zoom),
                                          static_cast<float>(SCREEN_WIDTH)/static_cast<float>(SCREEN_HEIGHT),
                                          0.1f, 100.0f);

        for (int tri = 0; tri < 12; ++tri) {
            const bool useA = tri % 2 == 0;
            const unsigned int program = useA ? shaderProgramA : shaderProgramB;

            const int mLoc = useA ? modelA : modelB;
            const int vLoc = useA ? viewA  : viewB;
            const int pLoc = useA ? projA  : projB;

            glUseProgram(program);
            glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(pLoc, 1, GL_FALSE, glm::value_ptr(proj));
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT,
                           reinterpret_cast<void*>(static_cast<size_t>(tri) * 3 * sizeof(unsigned int)));
        }
        // swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgramA);
    glDeleteProgram(shaderProgramB);

    glfwTerminate();
    return 0;
}

unsigned int createShaderProgram(const std::string& vertexFile, const std::string& fragmentFile ) {
    const std::string vertexSource = readShaderFromFile(vertexFile);
    const std::string fragmentSource = readShaderFromFile(fragmentFile);

    const unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    const unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

    const unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);


    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

unsigned int compileShader(const unsigned int type, const std::string& source) {
    const unsigned int shader = glCreateShader(type);
    const char* shaderSource = source.c_str();
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return shader;
}

std::string readShaderFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open shader file " << filename << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void framebuffer_size_callback(GLFWwindow* window, const int width, const int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FRONT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACK, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
}


void mouse_callback(GLFWwindow*, double xposIn, double yposIn) {
    float x = static_cast<float>(xposIn);
    float y = static_cast<float>(yposIn);

    if (!rotateMouseActive) {
        lastX = x; lastY = y;
        return;
    }

    if (firstMouse) { lastX = x; lastY = y; firstMouse = false; }

    float xoffset = (x - lastX) * 0.1f;
    float yoffset = (lastY - y) * 0.1f;
    lastX = x; lastY = y;

    camera.ProcessMouseMovement(xoffset, yoffset);
}


void scroll_callback(GLFWwindow*, double, double yoffset) {
    if (rotateMouseActive) return;
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            rotateMouseActive = true;
            firstMouse = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            double x, y; glfwGetCursorPos(window, &x, &y);
            lastX = static_cast<float>(x);
            lastY = static_cast<float>(y);
        } else if (action == GLFW_RELEASE) {
            rotateMouseActive = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            double x, y; glfwGetCursorPos(window, &x, &y);
            lastX = static_cast<float>(x);
            lastY = static_cast<float>(y);
            firstMouse = true;
        }
    }
}