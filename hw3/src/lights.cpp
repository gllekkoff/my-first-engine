//
// Created by gllekk on 10/24/25.
//
#include "lights.h"
#include <glm/gtc/matrix_transform.hpp>

void LightingSystem::apply(Shader& lightingShader, const glm::vec3& viewPos) const
{
    lightingShader.use();
    lightingShader.setVec3("material_diffuse",  material.diffuse);
    lightingShader.setVec3("material_specular", material.specular);
    lightingShader.setFloat("material_shininess", material.shininess);
    lightingShader.setVec3("viewPos", viewPos);

    lightingShader.setVec3("dirLight.direction", dir.direction);
    lightingShader.setVec3("dirLight.color",     dir.color);
    lightingShader.setFloat("dirLight.intensity", dir.intensity);

    for (int i=0;i<2;++i) {
        std::string base = "pointLights[" + std::to_string(i) + "].";
        const PointLight& L = points[i];
        lightingShader.setVec3( base+"position",  L.position );
        lightingShader.setVec3( base+"color",     L.color );
        lightingShader.setFloat(base+"intensity", L.intensity );
        lightingShader.setFloat(base+"constant",  L.constant );
        lightingShader.setFloat(base+"linear",    L.linear );
        lightingShader.setFloat(base+"quadratic", L.quadratic );
    }

    lightingShader.setVec3("spotLight.position",   spot.position);
    lightingShader.setVec3("spotLight.direction",  spot.direction);
    lightingShader.setVec3("spotLight.color",      spot.color);
    lightingShader.setFloat("spotLight.intensity", spot.intensity);
    lightingShader.setFloat("spotLight.cutOff",      spot.cutOff);
    lightingShader.setFloat("spotLight.outerCutOff", spot.outerCutOff);
    lightingShader.setFloat("spotLight.constant",    spot.constant);
    lightingShader.setFloat("spotLight.linear",      spot.linear);
    lightingShader.setFloat("spotLight.quadratic",   spot.quadratic);
}

void LightingSystem::drawMarkers(Shader& lampShader, const Model& glyph,
                                 const glm::mat4& P, const glm::mat4& V) const
{
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
