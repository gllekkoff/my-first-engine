#include "lights.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

LightingSystem::LightingSystem() : shadowMapFBO(0), shadowMap(0) {
}

LightingSystem::~LightingSystem() {
    if (shadowMapFBO) glDeleteFramebuffers(1, &shadowMapFBO);
    if (shadowMap) glDeleteTextures(1, &shadowMap);
}

void LightingSystem::initShadowMap() {
    glGenFramebuffers(1, &shadowMapFBO);

    glGenTextures(1, &shadowMap);
    glBindTexture(GL_TEXTURE_2D, shadowMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR: Shadow framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



glm::mat4 LightingSystem::getLightSpaceMatrix() const {
    constexpr float near_plane = 0.1f;
    constexpr float far_plane = 300.0f;
    constexpr float orthoSize = 250.0f;
    const glm::mat4 lightProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize,
                                           near_plane, far_plane);
    const glm::vec3 lightPos = -glm::normalize(dir.direction) * 100.0f;
    const glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    return lightProjection * lightView;
}


void LightingSystem::renderShadowMap(Shader& depthShader, const std::function<void(Shader&)>& renderScene) {
    glm::mat4 lightSpaceMatrix = getLightSpaceMatrix();

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    glCullFace(GL_FRONT);

    depthShader.use();
    depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

    renderScene(depthShader);

    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}

void LightingSystem::bindShadowMap(Shader& shader, int textureUnit) {
    shader.use();
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, shadowMap);
    shader.setInt("shadowMap", textureUnit);
    shader.setMat4("lightSpaceMatrix", getLightSpaceMatrix());
}

void LightingSystem::apply(Shader& lightingShader, const glm::vec3& viewPos) const {
    lightingShader.use();
    lightingShader.setVec3("material_diffuse", material.diffuse);
    lightingShader.setVec3("material_specular", material.specular);
    lightingShader.setFloat("material_shininess", material.shininess);
    lightingShader.setVec3("viewPos", viewPos);

    lightingShader.setVec3("dirLight.direction", dir.direction);
    lightingShader.setVec3("dirLight.color", dir.color);
    lightingShader.setFloat("dirLight.intensity", dir.intensity);

    for (int i=0; i<2; ++i) {
        std::string base = "pointLights[" + std::to_string(i) + "].";
        const PointLight& L = points[i];
        lightingShader.setVec3(base+"position", L.position);
        lightingShader.setVec3(base+"color", L.color);
        lightingShader.setFloat(base+"intensity", L.intensity);
        lightingShader.setFloat(base+"constant", L.constant);
        lightingShader.setFloat(base+"linear", L.linear);
        lightingShader.setFloat(base+"quadratic", L.quadratic);
    }

    lightingShader.setVec3("spotLight.position", spot.position);
    lightingShader.setVec3("spotLight.direction", spot.direction);
    lightingShader.setVec3("spotLight.color", spot.color);
    lightingShader.setFloat("spotLight.intensity", spot.intensity);
    lightingShader.setFloat("spotLight.cutOff", spot.cutOff);
    lightingShader.setFloat("spotLight.outerCutOff", spot.outerCutOff);
    lightingShader.setFloat("spotLight.constant", spot.constant);
    lightingShader.setFloat("spotLight.linear", spot.linear);
    lightingShader.setFloat("spotLight.quadratic", spot.quadratic);
}

void LightingSystem::drawMarkers(Shader& lampShader, const Model& glyph,
                                 const glm::mat4& P, const glm::mat4& V) const {
    lampShader.use();
    lampShader.setMat4("projection", P);
    lampShader.setMat4("view", V);
    glm::mat4 M(1.0f);

    M = glm::translate(glm::mat4(1.0f), points[0].position);
    M = glm::scale(M, glm::vec3(0.02f));
    lampShader.setMat4("model", M);
    lampShader.setVec3("lampColor", points[0].color);
    glyph.Draw(lampShader);

    M = glm::translate(glm::mat4(1.0f), points[1].position);
    M = glm::scale(M, glm::vec3(0.02f));
    lampShader.setMat4("model", M);
    lampShader.setVec3("lampColor", points[1].color);
    glyph.Draw(lampShader);

    M = glm::translate(glm::mat4(1.0f), -glm::normalize(dir.direction) * 80.0f);
    M = glm::scale(M, glm::vec3(0.02f));
    lampShader.setMat4("model", M);
    lampShader.setVec3("lampColor", dir.color);
    glyph.Draw(lampShader);
}

