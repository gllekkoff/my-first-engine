//
// Created by gllekk on 12/11/25.
//
#include "dithering.h"
#include <imgui.h>

DitheringEffect::DitheringEffect()
    : enabled(false), strength(1.0f), levels(8) {
    bayerTexture = generateBayerTexture();
}

DitheringEffect::~DitheringEffect() {
    glDeleteTextures(1, &bayerTexture);
}

unsigned int DitheringEffect::generateBayerTexture() {
    const unsigned char bayer8x8[64] = {
        0,  32,  8, 40,  2, 34, 10, 42,
        48, 16, 56, 24, 50, 18, 58, 26,
        12, 44,  4, 36, 14, 46,  6, 38,
        60, 28, 52, 20, 62, 30, 54, 22,
        3,  35, 11, 43,  1, 33,  9, 41,
        51, 19, 59, 27, 49, 17, 57, 25,
        15, 47,  7, 39, 13, 45,  5, 37,
        63, 31, 55, 23, 61, 29, 53, 21
    };

    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 8, 8, 0, GL_RED,
                 GL_UNSIGNED_BYTE, bayer8x8);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return textureID;
}

void DitheringEffect::bind(Shader& shader, int textureUnit) const {
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, bayerTexture);
    shader.setInt("bayerTexture", textureUnit);
}

void DitheringEffect::applySettings(Shader& shader) const {
    shader.setBool("enableDithering", enabled);
    shader.setFloat("ditherStrength", strength);
    shader.setInt("ditherLevels", levels);
}

void DitheringEffect::renderImGui() {
    if (ImGui::CollapsingHeader("Dithering Settings")) {
        ImGui::Checkbox("Enable Dithering", &enabled);
        ImGui::SliderFloat("Dither Strength", &strength, 0.0f, 2.0f);
        ImGui::SliderInt("Color Levels", &levels, 2, 32);
    }
}
