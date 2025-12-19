//
// Created by gllekk on 11/14/25.
//
#include "model.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include <iostream>
#include <stb_image.h>

Mesh::Mesh(const std::vector<Vertex>& vert, const std::vector<unsigned int>& inds, const std::vector<Texture>& texts) :
    vertices(vert), indices(inds), textures(texts) {
    setupMesh();
}

void Mesh::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Normal)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, TexCoords)));
    glBindVertexArray(0);
}

void Mesh::Draw(const Shader& shader) const {
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    for(unsigned int i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        std::string number;
        std::string name = textures[i].type;
        if(name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if(name == "texture_specular")
            number = std::to_string(specularNr++);
        shader.setInt("material." + name + number, i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}

unsigned int Mesh::getVao() const {
    return VAO;
}

Model::Model(const std::string& path) {
    loadModel(path);
}

void Model::loadModel(const std::string& path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs |
                                            aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices |
                                            aiProcess_ImproveCacheLocality | aiProcess_RemoveRedundantMaterials);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);
}

void Model::processNode(void* node, const void* scene) {
    const aiNode* ainode = static_cast<const aiNode*>(node);
    const aiScene* aiscene = static_cast<const aiScene*>(scene);
    for (unsigned int i = 0; i < ainode->mNumMeshes; i++) {
        const aiMesh* mesh = aiscene->mMeshes[ainode->mMeshes[i]];
        meshes.push_back(processMesh((void*)mesh, (void*)aiscene));
    }
    for (unsigned int i = 0; i < ainode->mNumChildren; i++) {
        processNode(ainode->mChildren[i], aiscene);
    }
}

Mesh Model::processMesh(void* mesh, const void* scene) {
    const aiMesh* aimesh = static_cast<const aiMesh*>(mesh);
    const aiScene* aiscene = static_cast<const aiScene*>(scene);
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < aimesh->mNumVertices; i++) {
        Vertex vertex;
        vertex.Position.x = aimesh->mVertices[i].x;
        vertex.Position.y = aimesh->mVertices[i].y;
        vertex.Position.z = aimesh->mVertices[i].z;
        if (aimesh->HasNormals()) {
            vertex.Normal.x = aimesh->mNormals[i].x;
            vertex.Normal.y = aimesh->mNormals[i].y;
            vertex.Normal.z = aimesh->mNormals[i].z;
        }
        if (aimesh->HasTextureCoords(0)) {
            vertex.TexCoords.x = aimesh->mTextureCoords[0][i].x;
            vertex.TexCoords.y = aimesh->mTextureCoords[0][i].y;
        } else vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        if(aimesh->HasTangentsAndBitangents()) {
            vertex.Tangent = glm::vec3(aimesh->mTangents[i].x, aimesh->mTangents[i].y, aimesh->mTangents[i].z);
            vertex.Bitangent = glm::vec3(aimesh->mBitangents[i].x, aimesh->mBitangents[i].y, aimesh->mBitangents[i].z);
        } else {
            vertex.Tangent = glm::vec3(0.0f);
            vertex.Bitangent = glm::vec3(0.0f);
        }
        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < aimesh->mNumFaces; i++) {
        const aiFace face = aimesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    aiMaterial* material = aiscene->mMaterials[aimesh->mMaterialIndex];
    std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(void* mat, int type, std::string typeName) {
    aiMaterial* material = static_cast<aiMaterial*>(mat);
    std::vector<Texture> textures;
    for(unsigned int i = 0; i < material->GetTextureCount(static_cast<aiTextureType>(type)); i++) {
        aiString str;
        material->GetTexture(static_cast<aiTextureType>(type), i, &str);
        bool skip = false;
        for(unsigned int j = 0; j < textures_loaded.size(); j++) {
            if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if(!skip) {
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }
    return textures;
}

void Model::Draw(const Shader& shader) const {
    for (auto& mesh : meshes) {
        mesh.Draw(shader);
    }
}

std::vector<Mesh> Model::getMeshes() const {
    return meshes;
}

unsigned int TextureFromFile(const char* path, const std::string& directory) {
    const std::string filename = directory + "/" + path;
    unsigned int textureID;
    glGenTextures(1, &textureID);
    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format = nrComponents == 1 ? GL_RED : (nrComponents == 3 ? GL_RGB : GL_RGBA);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    return textureID;
}
