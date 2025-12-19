//
// Created by gllekk on 10/9/25.
//

#ifndef MY_FIRST_ENGINE_MODEL_H
#define MY_FIRST_ENGINE_MODEL_H

#include <string>
#include <vector>
#include "shader.h"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "pbrMaterial.h"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    Mesh(const std::vector<Vertex>& vert, const std::vector<unsigned int>& inds, const std::vector<Texture>& texts);
    void Draw(const Shader& shader) const;
    unsigned int getVao() const;

private:
    unsigned int VBO, VAO, EBO;
    void setupMesh();
};

class Model {
public:
    explicit Model(const std::string& path);
    void Draw(const Shader& shader) const;
    std::vector<Mesh> getMeshes() const;
    bool isTransparent = false;
    glm::vec3 position = glm::vec3(0.0f);
    PBRMaterial pbrMaterial;
    float scale = 1.0f;
    glm::vec3 rotation = glm::vec3(0.0f);
    float getDistanceToCamera(const glm::vec3& cameraPos) const {
        return glm::length(cameraPos - position);
    }

private:
    std::vector<Mesh> meshes;
    std::string directory;
    std::vector<Texture> textures_loaded;

    void loadModel(const std::string& path);
    void processNode(void* node, const void* scene);
    Mesh processMesh(void* mesh, const void* scene);
    std::vector<Texture> loadMaterialTextures(void* mat, int type, std::string typeName);
};

unsigned int TextureFromFile(const char *path, const std::string &directory);

#endif
