#include "main.h"


Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool rotateMouseActive = false;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL HW4", NULL, NULL);
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

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

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

    Model headModel1("textures/head/head.OBJ");
    headModel1.isTransparent = false;
    headModel1.position = glm::vec3(-1.5f, -1.0f, 0.0f);

    Model headModel2("textures/head/head.OBJ");
    headModel2.isTransparent = true;
    headModel2.position = glm::vec3(1.5f, -1.0f, 0.0f);

    std::vector opaqueModels = { &headModel1 };
    std::vector transparentModels = { &headModel2 };

    Shader mainShader("shaders/lightning/lightningVertex.glsl", "shaders/lightning/lightningFragment.glsl");
    Shader lamp("shaders/lamp/lampVertex.glsl", "shaders/lamp/lampFragment.glsl");
    Shader depthShader("shaders/depthShadow/shadowDepth_vertex.glsl", "shaders/depthShadow/shadowDepth_frag.glsl");

    Model lightGlyph("textures/sphere_uv/sphere.obj");

    LightingSystem myLight;
    myLight.initShadowMap();

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

    float shadowBias = 0.005f;
    bool usePCF = false;

    while (!glfwWindowShouldClose(window)) {
        timeManager.beginFrame();
        processInput(window, timeManager.getDeltaTime());

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Shadow Settings");
        ImGui::SliderFloat("Shadow Bias", &shadowBias, 0.001f, 0.05f);
        ImGui::Checkbox("Use PCF", &usePCF);
        ImGui::End();

        auto renderSceneGeometry = [&](Shader& shader) {
            renderFloor(shader, planeMesh);
            for (auto* model : opaqueModels) {
                glm::mat4 M = glm::translate(glm::mat4(1.0f), model->position);
                M = glm::scale(M, glm::vec3(5.0f));
                shader.setMat4("model", M);
                model->Draw(shader);
            }
            for (auto* model : transparentModels) {
                glm::mat4 M = glm::translate(glm::mat4(1.0f), model->position);
                M = glm::scale(M, glm::vec3(5.0f));
                shader.setMat4("model", M);
                model->Draw(shader);
            }
        };

        myLight.renderShadowMap(depthShader, renderSceneGeometry);

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mainShader.use();
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom),
                                                static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
                                                0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        mainShader.setMat4("projection", projection);
        mainShader.setMat4("view", view);

        myLight.bindShadowMap(mainShader, 5);
        myLight.apply(mainShader, camera.Pos);

        mainShader.setFloat("shadowBias", shadowBias);
        mainShader.setBool("usePCF", usePCF);

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        setupFloorMaterial(mainShader, groundTexture);
        renderFloor(mainShader, planeMesh);

        setupModelMaterial(mainShader);
        mainShader.setFloat("materialAlpha", 1.0f);
        for (auto* model : opaqueModels) {
            glm::mat4 M = glm::translate(glm::mat4(1.0f), model->position);
            M = glm::scale(M, glm::vec3(5.0f));
            mainShader.setMat4("model", M);
            model->Draw(mainShader);
        }

        std::sort(transparentModels.begin(), transparentModels.end(),
            [&](const Model* a, const Model* b) {
                return a->getDistanceToCamera(camera.Pos) > b->getDistanceToCamera(camera.Pos);
            });

        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        mainShader.setFloat("materialAlpha", 0.5f);
        for (auto* model : transparentModels) {
            glm::mat4 M = glm::translate(glm::mat4(1.0f), model->position);
            M = glm::scale(M, glm::vec3(5.0f));
            mainShader.setMat4("model", M);
            model->Draw(mainShader);
        }

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        lamp.use();
        lamp.setFloat("emissiveIntensity", 1.0f);
        myLight.drawMarkers(lamp, lightGlyph, projection, view);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        timeManager.endFrame();
        char title[256];
        std::snprintf(title, sizeof(title), "Frame Time: %.2f ms | FPS: %u",
                      timeManager.getFrameTimeMs(), timeManager.getFps());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

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

void setupFloorMaterial(Shader& shader, const unsigned int groundTexture) {
    shader.setBool("useFloorTiling", true);
    shader.setBool("useModelTexture", false);
    shader.setFloat("floorTileScale", 10.0f);
    shader.setVec3("material.diffuse", glm::vec3(1.0f));
    shader.setVec3("material.specular", glm::vec3(0.2f));
    shader.setFloat("material.shininess", 16.0f);
    shader.setInt("floorTexture", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, groundTexture);
}

void setupModelMaterial(Shader& shader) {
    shader.setBool("useFloorTiling", false);
    shader.setBool("useModelTexture", true);
    shader.setVec3("material.specular", glm::vec3(0.35f));
    shader.setFloat("material.shininess", 32.0f);
}

void renderFloor(Shader& shader, const Mesh& planeMesh) {
    glm::mat4 M(1.0f);
    M = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    shader.setMat4("model", M);
    planeMesh.Draw(shader);
}

void renderModels(Shader& shader, const Model& myModel) {
    glm::mat4 M = glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, -2.0f, 0.0f));
    M = glm::scale(M, glm::vec3(5.0f));
    shader.setMat4("model", M);
    myModel.Draw(shader);

    M = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, -2.0f, 0.0f));
    M = glm::scale(M, glm::vec3(5.0f));
    shader.setMat4("model", M);
    myModel.Draw(shader);
}
