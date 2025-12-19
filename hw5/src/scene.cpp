//
// Created by gllekk on 12/11/25.
//
#include "scene.h"
#include <algorithm>
#include <glm/ext/matrix_transform.hpp>

Scene::Scene() : floorMesh(nullptr), floorTexture(0) {}

void Scene::addModel(Model* model, bool transparent) {
    if (transparent) {
        transparentModels.push_back(model);
    } else if (model->pbrMaterial.type == MaterialType::PBR) {
        pbrModels.push_back(model);
    } else {
        opaqueModels.push_back(model);
    }
}

void Scene::renderPBR(Shader& pbrShader) {
    pbrShader.setBool("useNormalMap", false);

    for (auto* model : pbrModels) {
        model->pbrMaterial.bindTextures();
        model->pbrMaterial.setUniforms(pbrShader);
        pbrShader.setBool("useNormalMap", model->pbrMaterial.textures.hasNormal);
        glm::mat4 M = glm::translate(glm::mat4(1.0f), model->position);
        M = glm::rotate(M, glm::radians(model->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        M = glm::rotate(M, glm::radians(model->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        M = glm::rotate(M, glm::radians(model->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        M = glm::scale(M, glm::vec3(model->scale));

        pbrShader.setMat4("model", M);
        model->Draw(pbrShader);
    }
}

void Scene::setFloor(const Mesh& mesh, unsigned int texture) {
    floorMesh = &mesh;
    floorTexture = texture;
}

void Scene::renderFloor(Shader& shader) {
    if (!floorMesh) return;

    shader.setBool("useFloorTiling", true);
    shader.setBool("useModelTexture", false);
    shader.setFloat("floorTileScale", 10.0f);
    shader.setVec3("material.diffuse", glm::vec3(1.0f));
    shader.setVec3("material.specular", glm::vec3(0.2f));
    shader.setFloat("material.shininess", 16.0f);
    shader.setInt("floorTexture", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, floorTexture);

    glm::mat4 M = glm::mat4(1.0f);
    shader.setMat4("model", M);
    floorMesh->Draw(shader);
}

void Scene::renderOpaque(Shader& shader) {
    shader.setBool("useFloorTiling", false);
    shader.setBool("useModelTexture", true);
    shader.setVec3("material.specular", glm::vec3(0.35f));
    shader.setFloat("material.shininess", 32.0f);
    shader.setFloat("materialAlpha", 1.0f);

    for (auto* model : opaqueModels) {
        glm::mat4 M = glm::translate(glm::mat4(1.0f), model->position);
        M = glm::rotate(M, glm::radians(model->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        M = glm::rotate(M, glm::radians(model->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        M = glm::rotate(M, glm::radians(model->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        M = glm::scale(M, glm::vec3(model->scale));
        shader.setMat4("model", M);
        model->Draw(shader);
    }
}


void Scene::renderTransparent(Shader& shader, const glm::vec3& cameraPos) {
    std::sort(transparentModels.begin(), transparentModels.end(),
        [&](const Model* a, const Model* b) {
            return a->getDistanceToCamera(cameraPos) > b->getDistanceToCamera(cameraPos);
        });

    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    shader.setFloat("materialAlpha", 0.5f);

    for (auto* model : transparentModels) {
        glm::mat4 M = glm::translate(glm::mat4(1.0f), model->position);
        M = glm::scale(M, glm::vec3(model->scale));
        shader.setMat4("model", M);
        model->Draw(shader);
    }

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

void Scene::renderAll(Shader& shader) {
    renderFloor(shader);

    for (auto* model : opaqueModels) {
        glm::mat4 M = glm::translate(glm::mat4(1.0f), model->position);
        M = glm::rotate(M, glm::radians(model->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        M = glm::rotate(M, glm::radians(model->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        M = glm::rotate(M, glm::radians(model->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        M = glm::scale(M, glm::vec3(model->scale));
        shader.setMat4("model", M);
        model->Draw(shader);
    }

    for (auto* model : pbrModels) {
        glm::mat4 M = glm::translate(glm::mat4(1.0f), model->position);
        M = glm::rotate(M, glm::radians(model->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        M = glm::rotate(M, glm::radians(model->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        M = glm::rotate(M, glm::radians(model->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        M = glm::scale(M, glm::vec3(model->scale));
        shader.setMat4("model", M);
        model->Draw(shader);
    }

    for (auto* model : transparentModels) {
        glm::mat4 M = glm::translate(glm::mat4(1.0f), model->position);
        M = glm::rotate(M, glm::radians(model->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        M = glm::rotate(M, glm::radians(model->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        M = glm::rotate(M, glm::radians(model->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        M = glm::scale(M, glm::vec3(model->scale));
        shader.setMat4("model", M);
        model->Draw(shader);
    }
}
