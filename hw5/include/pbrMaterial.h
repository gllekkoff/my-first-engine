//
// Created by gllekk on 12/19/25.
//

#ifndef MY_FIRST_ENGINE_PBRMATERIAL_H
#define MY_FIRST_ENGINE_PBRMATERIAL_H

#include <glm/glm.hpp>
#include <string>

enum class MaterialType {
    BLINN_PHONG,
    PBR,
    EMISSIVE
};

struct PBRTextures {
    unsigned int albedo;
    unsigned int normal;
    unsigned int metallic;
    unsigned int roughness;
    unsigned int ao;

    bool hasAlbedo;
    bool hasNormal;
    bool hasMetallic;
    bool hasRoughness;
    bool hasAO;

    PBRTextures() : albedo(0), normal(0), metallic(0), roughness(0), ao(0),
                    hasAlbedo(false), hasNormal(false), hasMetallic(false),
                    hasRoughness(false), hasAO(false) {}
};

class PBRMaterial {
public:
    MaterialType type;

    glm::vec3 albedo;
    float metallic;
    float roughness;
    float ao;

    PBRTextures textures;

    PBRMaterial();

    void bindTextures() const;
    void setUniforms(class Shader& shader) const;
};

unsigned int LoadPBRTexture(const char* path);


#endif
