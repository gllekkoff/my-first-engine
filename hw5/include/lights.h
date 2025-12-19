//
// Created by gllekk on 10/24/25.
//

#ifndef MY_FIRST_ENGINE_LIGHTS_H
#define MY_FIRST_ENGINE_LIGHTS_H

#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <functional>
#include "shader.h"
#include "model.h"
#include <imgui.h>
#include <glm/glm.hpp>

struct DirLight {
    glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);
    glm::vec3 color = glm::vec3(1.0f);
    float intensity = 0.5f;
    bool enabled = true;
};

struct PointLight {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 color = glm::vec3(1.0f);
    float intensity = 1.0f;
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
    bool enabled = true;

    void setRange(float range) {
        if (range <= 7) { constant = 1.0f; linear = 0.7f; quadratic = 1.8f; }
        else if (range <= 13) { constant = 1.0f; linear = 0.35f; quadratic = 0.44f; }
        else if (range <= 20) { constant = 1.0f; linear = 0.22f; quadratic = 0.20f; }
        else if (range <= 32) { constant = 1.0f; linear = 0.14f; quadratic = 0.07f; }
        else if (range <= 50) { constant = 1.0f; linear = 0.09f; quadratic = 0.032f; }
        else if (range <= 100) { constant = 1.0f; linear = 0.045f; quadratic = 0.0075f; }
        else { constant = 1.0f; linear = 0.022f; quadratic = 0.0019f; }
    }

    float getRange() const {
        if (quadratic > 1.0f) return 7.0f;
        if (quadratic > 0.3f) return 13.0f;
        if (quadratic > 0.1f) return 20.0f;
        if (quadratic > 0.04f) return 32.0f;
        if (quadratic > 0.01f) return 50.0f;
        if (quadratic > 0.003f) return 100.0f;
        return 200.0f;
    }
};

struct SpotLight {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);
    glm::vec3 color = glm::vec3(1.0f);
    float intensity = 1.0f;
    float cutOff = glm::cos(glm::radians(12.5f));
    float outerCutOff = glm::cos(glm::radians(17.5f));
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
    bool enabled = true;

    void setRange(float range) {
        if (range <= 7) { constant = 1.0f; linear = 0.7f; quadratic = 1.8f; }
        else if (range <= 13) { constant = 1.0f; linear = 0.35f; quadratic = 0.44f; }
        else if (range <= 20) { constant = 1.0f; linear = 0.22f; quadratic = 0.20f; }
        else if (range <= 32) { constant = 1.0f; linear = 0.14f; quadratic = 0.07f; }
        else if (range <= 50) { constant = 1.0f; linear = 0.09f; quadratic = 0.032f; }
        else if (range <= 100) { constant = 1.0f; linear = 0.045f; quadratic = 0.0075f; }
        else { constant = 1.0f; linear = 0.022f; quadratic = 0.0019f; }
    }

    float getRange() const {
        if (quadratic > 1.0f) return 7.0f;
        if (quadratic > 0.3f) return 13.0f;
        if (quadratic > 0.1f) return 20.0f;
        if (quadratic > 0.04f) return 32.0f;
        if (quadratic > 0.01f) return 50.0f;
        if (quadratic > 0.003f) return 100.0f;
        return 200.0f;
    }
};

struct MaterialSimple {
    glm::vec3 diffuse {0.95f};
    glm::vec3 specular {0.35f};
    float shininess = 32.0f;
};

class LightingSystem {
public:
    DirLight dir;
    PointLight points[2];
    SpotLight spot;
    MaterialSimple material;

    unsigned int shadowMapFBO;
    unsigned int shadowMap;
    const unsigned int SHADOW_WIDTH = 8192;
    const unsigned int SHADOW_HEIGHT = 8192;
    float shadowOrthoSize = 250.0f;

    LightingSystem();
    ~LightingSystem();

    void initShadowMap();
    [[nodiscard]] glm::mat4 getLightSpaceMatrix() const;
    void renderShadowMap(Shader& depthShader, const std::function<void(Shader&)>& renderScene);
    void bindShadowMap(Shader& shader, int textureUnit = 5);

    void apply(Shader& lightingShader, const glm::vec3& viewPos) const;
    void drawMarkers(Shader& lampShader, const Model& glyph,
                     const glm::mat4& P, const glm::mat4& V) const;

    void renderImGui();
    unsigned int getShadowMapTexture() const { return shadowMap; }
};

#endif
