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

#define PERF false
#define INSTANCING true
#define STEP_2_MODEL false

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

    // Bool parameters

    // To log frametime and time
#if PERF
    PerformanceLogger logger(1.0f);
#endif

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
    Shader planeShader("shaders/plane/planeVertex.glsl", "shaders/plane/planeFragment.glsl");
    const unsigned int groundTexture = TextureFromFile("planeGrass.jpg", "textures/plane");

#if STEP_2_MODEL
    Shader modelShader("shaders/modelHead/modelVertex.glsl", "shaders/modelHead/modelFragment.glsl");
    Model myModel("textures/head/head.OBJ");
#endif

    glm::mat4 modelMatrices[amount];

#if INSTANCING
    Shader instanceShader("shaders/instance/instanceShader.glsl", "shaders/modelHead/modelFragment.glsl");
#else
    Shader nonInstanceShader("shaders/modelHead/modelVertex.glsl", "shaders/modelHead/modelFragment.glsl");
#endif

    const Model cube("textures/cube/cube.obj");
    loadCubesStep3(cube, modelMatrices);


    while (!glfwWindowShouldClose(window))
    {
        timeManager.beginFrame();

        processInput(window, timeManager.getDeltaTime());

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom),
                                            static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
                                            0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();


        planeShader.use();
        planeShader.setMat4("view", view);
        planeShader.setMat4("projection", projection);
        planeShader.setMat4("model", glm::mat4(1.0f));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, groundTexture);
        planeShader.setInt("floorTexture", 0);
        planeMesh.Draw(planeShader);

#if STEP_2_MODEL
        modelShader.use();
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", view);
        modelShader.setMat4("modelHead", glm::mat4(1.0f));
        myModel.Draw(modelShader);
#endif

#if INSTANCING
        instanceShader.use();
        instanceShader.setMat4("projection", projection);
        instanceShader.setMat4("view", view);
        for(unsigned int i = 0; i < cube.getMeshes().size(); i++)
        {
            glBindVertexArray(cube.getMeshes()[i].getVao());
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, cube.getMeshes()[i].textures[0].id);
            glDrawElementsInstanced(
                GL_TRIANGLES,
                cube.getMeshes()[i].indices.size(),
                GL_UNSIGNED_INT,
                0,
                amount
            );
            glBindVertexArray(0);
        }
#else
        nonInstanceShader.use();
        nonInstanceShader.setMat4("projection", projection);
        nonInstanceShader.setMat4("view", view);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cube.getMeshes()[0].textures[0].id);
        for (const auto & modelMatrice : modelMatrices)
        {
            nonInstanceShader.setMat4("model", modelMatrice);
            cube.getMeshes()[0].Draw(nonInstanceShader);
        }
#endif

        glfwSwapBuffers(window);
        glfwPollEvents();

        timeManager.endFrame();

#if PERF
        logger.update(timeManager.getDeltaTime(), timeManager.getFrameTimeMs());
#endif

        char title[256];
        sprintf(title, "Frame Time: %.2f ms | FPS: %u", timeManager.getFrameTimeMs(), timeManager.getFps());
        glfwSetWindowTitle(window, title);
    }

#if PERF
    const std::string path("../dataPlots/performance_non_instanced1k.csv");
    if (logger.writeToFile(path))
        std::cout << "log written to " << path << ";" << std::endl;
#endif

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

void loadCubesStep3(const Model& cube, glm::mat4 modelMatrix[amount]) {
    srand(static_cast<unsigned int>(glfwGetTime()));
    for (unsigned int i = 0; i < amount; i++)
    {
        auto model = glm::mat4(1.0f);

        constexpr float range = 100.0f;
        const float x = random() % static_cast<int>(range * 75) / 100.0f - range;
        const float y = random() % static_cast<int>(range * 75) / 100.0f;
        const float z = random() % static_cast<int>(range * 75) / 100.0f - range;
        model = glm::translate(model, glm::vec3(x, y, z));

        const float scale = static_cast<float>(random() % 50 / 100.0 + 0.05);
        model = glm::scale(model, glm::vec3(scale));

        const float rotAngle = static_cast<float>(random() % 360);
        model = glm::rotate(model, glm::radians(rotAngle), glm::vec3(0.4f, 0.6f, 0.8f));

        modelMatrix[i] = model;
    }

    unsigned int instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrix[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    for(unsigned int i = 0; i < cube.getMeshes().size(); i++)
    {
        const unsigned int VAO = cube.getMeshes()[i].getVao();
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), static_cast<void *>(0));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), reinterpret_cast<void *>(sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), reinterpret_cast<void *>(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), reinterpret_cast<void *>(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
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
