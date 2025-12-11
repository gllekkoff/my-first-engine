#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "shader.h"
#include "cameraClass.h"
#include "model.h"
#include "timeClass.h"
#include "performanceLog.h"
#include "lights.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
constexpr unsigned int SCR_WIDTH = 1280;
constexpr unsigned int SCR_HEIGHT = 720;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool rotateMouseActive = false;

constexpr unsigned int amount = 10000;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mode);
void processInput(GLFWwindow *window, float deltaTime);
unsigned int LoadTexture(char const *path, const std::string& directory);
void loadCubesStep3(const Model& cube, glm::mat4 modelMatrix[amount]);

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Project", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    TimeManager timeManager;

    std::vector<Vertex> planeVertices = {
        { glm::vec3( 250.0f, -5.0f,  250.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(25.0f,  0.0f) },
        { glm::vec3(-250.0f, -5.0f,  250.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2( 0.0f,  0.0f) },
        { glm::vec3(-250.0f, -5.0f, -250.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2( 0.0f, 25.0f) },
        { glm::vec3( 250.0f, -5.0f, -250.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(25.0f, 25.0f) }
    };
    std::vector<unsigned int> planeIndices = {
        0, 1, 2,
        0, 2, 3
    };
    const Mesh planeMesh(planeVertices, planeIndices, {});
    const unsigned int groundTexture = TextureFromFile("planeGrass.jpg", "textures/plane");
    Model myModel("textures/head/head.OBJ");
    Shader lighting("shaders/lightning/lightningVertex.glsl", "shaders/lightning/lightningFragment.glsl");
    Shader lamp("shaders/lamp/lampVertex.glsl", "shaders/lamp/lampFragment.glsl");
    Model lightGlyph("textures/sphere_uv/sphere.obj");
    LightingSystem myLight;

    myLight.dir.direction = glm::vec3(-0.25f,-1.0f,-0.2f);
    myLight.dir.color     = glm::vec3(1.0f,0.97f,0.92f);
    myLight.dir.intensity = 0.5f;

    myLight.points[0].position   = glm::vec3(-2.0f, 0.0f, 0.0f);
    myLight.points[0].color      = glm::vec3(1.0f, 0.25f, 0.25f);
    myLight.points[0].intensity  = 1.0f;
    myLight.points[1].position   = glm::vec3(2.0f, 0.0f, 0.0f);
    myLight.points[1].color      = glm::vec3(0.25f, 0.8f, 1.0f);
    myLight.points[1].intensity  = 1.0f;
    myLight.spot.intensity = 0.6f;

    while (!glfwWindowShouldClose(window))
    {
        timeManager.beginFrame();
        processInput(window, timeManager.getDeltaTime());

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom),
            static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();

        myLight.spot.position  = camera.Pos;
        myLight.spot.direction = camera.Front;

        lighting.use();
        lighting.setMat4("projection", projection);
        lighting.setMat4("view", view);
        myLight.apply(lighting, camera.Pos);

        lighting.setBool("useModelTexture", false);
        lighting.setBool("useFloorTiling",  true);
        lighting.setFloat("floorTileScale", 10.0f);
        lighting.setVec3("material.diffuse",  glm::vec3(1.0f));
        lighting.setVec3("material.specular", glm::vec3(0.2f));
        lighting.setFloat("material.shininess", 16.0f);
        lighting.setInt("floorTexture", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, groundTexture);


        glm::mat4 M(1.0f);
        M = glm::translate(M, glm::vec3(0.0f, 0.0f, 0.0f));
        lighting.setMat4("model", M);
        planeMesh.Draw(lighting);

        lighting.setBool("useFloorTiling", false);
        lighting.setBool("useModelTexture", true);
        lighting.setVec3("material.specular", glm::vec3(0.35f));
        lighting.setFloat("material.shininess", 32.0f);

        M = glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, -0.2f, 0.0f));
        M = glm::scale(M, glm::vec3(0.9f));
        lighting.setMat4("model", M);
        myModel.Draw(lighting);

        M = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, -0.2f, 0.0f));
        M = glm::scale(M, glm::vec3(0.9f));
        lighting.setMat4("model", M);
        myModel.Draw(lighting);


        lamp.use();
        lamp.setFloat("emissiveIntensity", 1.0f);
        myLight.drawMarkers(lamp, lightGlyph, projection, view);

        glfwSwapBuffers(window);
        glfwPollEvents();

        timeManager.endFrame();
        char title[256];
        std::snprintf(title, sizeof(title), "Frame Time: %.2f ms | FPS: %u",
                      timeManager.getFrameTimeMs(), timeManager.getFps());
        glfwSetWindowTitle(window, title);
    }


    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window, const float deltaTime)
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

void framebuffer_size_callback(GLFWwindow* window, const int width, const int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, const double xposIn, const double yposIn)
{
    if (!rotateMouseActive) {
        return;
    }

    const float xpos = static_cast<float>(xposIn);
    const float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    const float xoffset = xpos - lastX;
    const float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, const double xoffset, const double yoffset)
{
}

void mouse_button_callback(GLFWwindow* window, const int button, const int action, int mode)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS)
        {
            rotateMouseActive = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            firstMouse = true;
        }
        else if (action == GLFW_RELEASE)
        {
            rotateMouseActive = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}

unsigned int TextureFromFile(const char* path, const std::string& directory){
    const std::string filename = directory + "/" + path;
    unsigned int textureID;
    glGenTextures(1, &textureID);
    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format = 0;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << filename.c_str() << std::endl;
        stbi_image_free(data);
        return 0;
    }

    return textureID;
}
