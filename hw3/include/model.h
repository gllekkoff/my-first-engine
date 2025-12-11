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

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

unsigned int TextureFromFile(const char *path, const std::string &directory);

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
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

    [[nodiscard]] unsigned int getVao() const;

private:
    unsigned VBO, VAO, EBO;
    void setupMesh();

};

class Model {
    std::vector<Mesh> meshes;
    std::string directory;

    std::vector<Texture> textures_loaded;

    Mesh processMesh(const aiMesh* mesh, const aiScene* scene);

    void processNode(const aiNode* node, const aiScene* scene);
    void loadModel(const std::string& path);

    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
public:
    Model(const std::string& path);

    void Draw(const Shader& shader) const;

    std::vector<Mesh> getMeshes() const;
};



#endif //MY_FIRST_ENGINE_MODEL_H