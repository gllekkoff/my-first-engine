//
// Created by gllekk on 12/11/25.
//

#ifndef MY_FIRST_ENGINE_SCENE_H
#define MY_FIRST_ENGINE_SCENE_H

#include <vector>
#include "model.h"
#include "shader.h"
#include "camera.h"

class Scene {
public:
    Scene();

    void addModel(Model* model, bool transparent = false);
    void renderOpaque(Shader& shader);
    void renderPBR(Shader& pbrShader);
    void renderTransparent(Shader& shader, const glm::vec3& cameraPos);
    void renderAll(Shader& shader);
    void setFloor(const Mesh& mesh, unsigned int texture);
    void renderFloor(Shader& shader);

private:
    std::vector<Model*> opaqueModels;
    std::vector<Model*> pbrModels;
    std::vector<Model*> transparentModels;
    const Mesh* floorMesh;
    unsigned int floorTexture;
};

#endif //MY_FIRST_ENGINE_SCENE_H