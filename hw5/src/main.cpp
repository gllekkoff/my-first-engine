#include "main.h"

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool rotateMouseActive = false;
int currentWidth = SCR_WIDTH;
int currentHeight = SCR_HEIGHT;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL HW4", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    HDRSystem hdrSystem(SCR_WIDTH, SCR_HEIGHT);
    Shader hdrShader("shaders/hdr/hdr_vertex.glsl",
                 "shaders/hdr/hdr_fragment.glsl");
    Shader pbrShader("shaders/pbr/pbr_vertex.glsl",
                     "shaders/pbr/pbr_fragment.glsl");

    glEnable(GL_DEPTH_TEST);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    TimeManager timeManager;
    Scene scene;
    RenderSettings renderSettings;
    DitheringEffect dithering;

    std::vector<Vertex> planeVertices = {
        { glm::vec3( 250.0f, -5.0f,  250.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(25.0f, 0.0f) },
        { glm::vec3(-250.0f, -5.0f,  250.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2( 0.0f, 0.0f) },
        { glm::vec3(-250.0f, -5.0f, -250.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2( 0.0f, 25.0f) },
        { glm::vec3( 250.0f, -5.0f, -250.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(25.0f, 25.0f) }
    };
    std::vector<unsigned int> planeIndices = { 0, 1, 2, 0, 2, 3 };
    Mesh planeMesh(planeVertices, planeIndices, {});
    unsigned int groundTexture = TextureFromFile("planeGrass.jpg", "textures/plane");
    scene.setFloor(planeMesh, groundTexture);

    // Model headModel1("textures/head/head.OBJ");
    // headModel1.position = glm::vec3(-7.5f, -1.0f, 0.0f);
    // scene.addModel(&headModel1, false);
    //
    // Model headModel2("textures/head/head.OBJ");
    // headModel2.position = glm::vec3(7.5f, -1.0f, 0.0f);
    // scene.addModel(&headModel2, true);

    // Model sphere1("textures/sphere_uv/sphere.obj");
    // sphere1.position = glm::vec3(-7.5f, 2.0f, 0.0f);
    // scene.addModel(&sphere1, false);
    //
    // Model sphere2("textures/sphere_uv/sphere.obj");
    // sphere2.position = glm::vec3(7.5f, 2.0f, 0.0f);
    // scene.addModel(&sphere2, false);

    Model pbrModel("textures/vinyl/vinyl.fbx");
    pbrModel.position = glm::vec3(0.0f, -3.0f, 0.0f);
    pbrModel.scale = 10.0f;
    pbrModel.rotation = glm::vec3(90.0f, 0.0f, 0.0f);
    pbrModel.pbrMaterial.type = MaterialType::PBR;

    pbrModel.pbrMaterial.textures.albedo = LoadPBRTexture("textures/vinyl/Textures/vinyl_basecolor.png");
    pbrModel.pbrMaterial.textures.hasAlbedo = true;

    pbrModel.pbrMaterial.textures.normal = LoadPBRTexture("textures/vinyl/Textures/vinyl_normal.png");
    pbrModel.pbrMaterial.textures.hasNormal = true;

    pbrModel.pbrMaterial.textures.metallic = LoadPBRTexture("textures/vinyl/Textures/vinyl_metal.png");
    pbrModel.pbrMaterial.textures.hasMetallic = true;

    pbrModel.pbrMaterial.textures.roughness = LoadPBRTexture("textures/vinyl/Textures/vinyl_roughness.png");
    pbrModel.pbrMaterial.textures.hasRoughness = true;

    pbrModel.pbrMaterial.textures.ao = LoadPBRTexture("textures/vinyl/Textures/vinyl_ambient.png");
    pbrModel.pbrMaterial.textures.hasAO = true;

    pbrModel.pbrMaterial.albedo = glm::vec3(0.3f, 0.3f, 0.3f);
    pbrModel.pbrMaterial.metallic = 0.0f;
    pbrModel.pbrMaterial.roughness = 0.1f;
    pbrModel.pbrMaterial.ao = 1.0f;

    scene.addModel(&pbrModel, false);

    Shader mainShader("shaders/lightning/lightningVertex.glsl",
                      "shaders/lightning/lightningFragment.glsl");
    Shader lamp("shaders/lamp/lampVertex.glsl",
                "shaders/lamp/lampFragment.glsl");
    Shader depthShader("shaders/depthShadow/shadowDepth_vertex.glsl",
                       "shaders/depthShadow/shadowDepth_frag.glsl");


    Model lightGlyph("textures/sphere_uv/sphere.obj");
    LightingSystem lighting;
    lighting.initShadowMap();

    lighting.dir.direction = glm::vec3(-0.5f, -1.0f, -0.5f);
    lighting.dir.color = glm::vec3(1.0f, 1.0f, 1.0f);
    lighting.dir.intensity = 1.0f;
    lighting.dir.enabled = true;

    lighting.shadowOrthoSize = 200.0f;

    lighting.spot.enabled = true;

    lighting.points[0].position = glm::vec3(-20.0f, 10.0f, 0.0f);
    lighting.points[0].color = glm::vec3(1.0f, 0.25f, 0.25f);
    lighting.points[0].intensity = 40.0f;
    lighting.points[1].position = glm::vec3(20.0f, 10.0f, 0.0f);
    lighting.points[1].color = glm::vec3(0.25f, 0.8f, 1.0f);
    lighting.points[1].intensity = 40.0f;
    lighting.points[0].enabled = false;
    lighting.points[1].enabled = false;

    while (!glfwWindowShouldClose(window)) {
        timeManager.beginFrame();
        processInput(window, timeManager.getDeltaTime());

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Rendering Settings");
        hdrSystem.renderImGui();
        renderSettings.renderImGui();
        dithering.renderImGui();
        lighting.renderImGui();
        ImGui::End();

        if (currentWidth != SCR_WIDTH || currentHeight != SCR_HEIGHT) {
            hdrSystem.resize(currentWidth, currentHeight);
        }

        lighting.renderShadowMap(depthShader, [&](Shader& shader) {
            scene.renderAll(shader);
        });

        hdrSystem.bindFramebuffer();

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lighting.spot.position  = camera.Pos;
        lighting.spot.direction = camera.Front;

        mainShader.use();

        mainShader.setVec3("material.diffuse", glm::vec3(0.8f, 0.2f, 0.2f));
        mainShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        mainShader.setFloat("material.shininess", 64.0f);
        mainShader.setBool("useModelTexture", false);

        glm::mat4 projection = glm::perspective(
            glm::radians(camera.zoom),
            static_cast<float>(currentWidth) / static_cast<float>(currentHeight),
            0.1f, 1000.0f
        );
        glm::mat4 view = camera.GetViewMatrix();
        mainShader.setMat4("projection", projection);
        mainShader.setMat4("view", view);

        lighting.bindShadowMap(mainShader, 5);
        lighting.apply(mainShader, camera.Pos);
        mainShader.setFloat("shadowBias", renderSettings.shadowBias);
        mainShader.setBool("usePCF", renderSettings.usePCF);

        dithering.bind(mainShader, 6);
        dithering.applySettings(mainShader);

        scene.renderFloor(mainShader);
        scene.renderOpaque(mainShader);

        pbrShader.use();
        pbrShader.setMat4("projection", projection);
        pbrShader.setMat4("view", view);

        pbrShader.setVec3("viewPos", camera.Pos);

        pbrShader.setVec3("dirLight.direction", lighting.dir.direction);
        pbrShader.setVec3("dirLight.color", lighting.dir.color);
        pbrShader.setFloat("dirLight.intensity", lighting.dir.intensity);

        for (int i = 0; i < 2; i++) {
            std::string base = "pointLights[" + std::to_string(i) + "].";
            pbrShader.setVec3(base + "position", lighting.points[i].position);
            pbrShader.setVec3(base + "color", lighting.points[i].color);
            pbrShader.setFloat(base + "intensity", lighting.points[i].intensity);
            pbrShader.setFloat(base + "constant", lighting.points[i].constant);
            pbrShader.setFloat(base + "linear", lighting.points[i].linear);
            pbrShader.setFloat(base + "quadratic", lighting.points[i].quadratic);
        }

        lighting.bindShadowMap(pbrShader, 5);
        pbrShader.setFloat("shadowBias", renderSettings.shadowBias);
        pbrShader.setBool("usePCF", renderSettings.usePCF);
        scene.renderPBR(pbrShader);

        mainShader.use();
        scene.renderTransparent(mainShader, camera.Pos);

        lamp.use();
        lamp.setFloat("emissiveIntensity", 1.0f);
        lighting.drawMarkers(lamp, lightGlyph, projection, view);

        hdrSystem.unbindFramebuffer();
        hdrSystem.renderToScreen(hdrShader);

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

void processInput(GLFWwindow *window, const float deltaTime) {
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

void mouse_callback(GLFWwindow* window, const double xposIn, const double yposIn) {
    if (!rotateMouseActive) return;

    const float xpos = static_cast<float>(xposIn);
    const float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
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

void mouse_button_callback(GLFWwindow* window, const int button, const int action, int mode) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            rotateMouseActive = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            firstMouse = true;
        }
        else if (action == GLFW_RELEASE) {
            rotateMouseActive = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}