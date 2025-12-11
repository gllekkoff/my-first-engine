//
// Created by gllekk on 10/24/25.
//

#ifndef MY_FIRST_ENGINE_LIGHTS_H
#define MY_FIRST_ENGINE_LIGHTS_H
#pragma once
#include <glm/glm.hpp>
#include <string>
#include "shader.h"
#include "model.h"

struct DirLight {
    glm::vec3 direction {-0.25f,-1.0f,-0.2f};
    glm::vec3 color     {1.0f, 0.97f, 0.92f};
    float     intensity = 0.5f;
};

struct PointLight {
    glm::vec3 position  {0.0f};
    glm::vec3 color     {1.0f};
    float     intensity = 1.0f;
    float     constant  = 1.0f;
    float     linear    = 0.14f;
    float     quadratic = 0.07f;
};

struct SpotLight {
    glm::vec3 position  {0.0f};
    glm::vec3 direction {0.0f,0.0f,-1.0f};
    glm::vec3 color     {1.0f,1.0f,0.9f};
    float     intensity = 0.6f;
    float     cutOff      = glm::cos(glm::radians(12.5f));
    float     outerCutOff = glm::cos(glm::radians(17.5f));
    float     constant  = 1.0f;
    float     linear    = 0.14f;
    float     quadratic = 0.07f;
};

struct MaterialSimple {
    glm::vec3 diffuse  {0.95f};
    glm::vec3 specular {0.35f};
    float     shininess = 32.0f;
};

class LightingSystem {
public:
    DirLight dir;
    PointLight points[2];
    SpotLight spot;
    MaterialSimple material;

    void apply(Shader& lightingShader, const glm::vec3& viewPos) const;

    void drawMarkers(Shader& lampShader, const Model& glyph,
                     const glm::mat4& P, const glm::mat4& V) const;
};


#endif //MY_FIRST_ENGINE_LIGHTS_H