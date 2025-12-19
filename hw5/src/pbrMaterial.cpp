//
// Created by gllekk on 12/19/25.
//
#include "shader.h"
#include <glad/glad.h>
#include "pbrMaterial.h"

#include <stb_image.h>

PBRMaterial::PBRMaterial()
    : type(MaterialType::BLINN_PHONG),
      albedo(0.8f, 0.8f, 0.8f),
      metallic(0.0f),
      roughness(0.5f),
      ao(1.0f) {}

void PBRMaterial::bindTextures() const {
    if (textures.hasAlbedo) {
        glActiveTexture(GL_TEXTURE0 + 7);
        glBindTexture(GL_TEXTURE_2D, textures.albedo);
    }
    if (textures.hasNormal) {
        glActiveTexture(GL_TEXTURE0 + 8);
        glBindTexture(GL_TEXTURE_2D, textures.normal);
    }
    if (textures.hasMetallic) {
        glActiveTexture(GL_TEXTURE0 + 9);
        glBindTexture(GL_TEXTURE_2D, textures.metallic);
    }
    if (textures.hasRoughness) {
        glActiveTexture(GL_TEXTURE0 + 10);
        glBindTexture(GL_TEXTURE_2D, textures.roughness);
    }
    if (textures.hasAO) {
        glActiveTexture(GL_TEXTURE0 + 11);
        glBindTexture(GL_TEXTURE_2D, textures.ao);
    }
}

void PBRMaterial::setUniforms(Shader& shader) const {
    shader.setVec3("pbrMaterial.albedo", albedo);
    shader.setFloat("pbrMaterial.metallic", metallic);
    shader.setFloat("pbrMaterial.roughness", roughness);
    shader.setFloat("pbrMaterial.ao", ao);

    shader.setBool("pbrMaterial.hasAlbedoMap", textures.hasAlbedo);
    shader.setBool("pbrMaterial.hasNormalMap", textures.hasNormal);
    shader.setBool("pbrMaterial.hasMetallicMap", textures.hasMetallic);
    shader.setBool("pbrMaterial.hasRoughnessMap", textures.hasRoughness);
    shader.setBool("pbrMaterial.hasAOMap", textures.hasAO);

    if (textures.hasAlbedo) shader.setInt("pbrMaterial.albedoMap", 7);
    if (textures.hasNormal) shader.setInt("pbrMaterial.normalMap", 8);
    if (textures.hasMetallic) shader.setInt("pbrMaterial.metallicMap", 9);
    if (textures.hasRoughness) shader.setInt("pbrMaterial.roughnessMap", 10);
    if (textures.hasAO) shader.setInt("pbrMaterial.aoMap", 11);
}

unsigned int LoadPBRTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Failed to load texture: " << path << std::endl;
        stbi_image_free(data);
    }
    return textureID;
}
