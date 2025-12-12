//
// Created by gllekk on 12/11/25.
//

#ifndef MY_FIRST_ENGINE_DITHERING_H
#define MY_FIRST_ENGINE_DITHERING_H

#include <glad/glad.h>
#include "shader.h"

class DitheringEffect {
public:
    DitheringEffect();
    ~DitheringEffect();

    void bind(Shader& shader, int textureUnit = 6) const;
    void applySettings(Shader& shader) const;
    void renderImGui();

    [[nodiscard]] bool isEnabled() const { return enabled; }

private:
    unsigned int bayerTexture;
    bool enabled;
    float strength;
    int levels;

    static unsigned int generateBayerTexture();
};

#endif //MY_FIRST_ENGINE_DITHERING_H