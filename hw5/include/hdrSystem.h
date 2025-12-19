//
// Created by gllekk on 12/19/25.
//

#ifndef MY_FIRST_ENGINE_HDRSYSTEM_H
#define MY_FIRST_ENGINE_HDRSYSTEM_H

#include <glad/glad.h>
#include "shader.h"

class HDRSystem {
public:
    HDRSystem(unsigned int width, unsigned int height);
    ~HDRSystem();

    void resize(unsigned int width, unsigned int height);
    void bindFramebuffer();
    void unbindFramebuffer();
    void renderToScreen(Shader& hdrShader);
    void renderImGui();

    [[nodiscard]] unsigned int getHDRTexture() const { return hdrColorBuffer; }
    [[nodiscard]] bool isEnabled() const { return enabled; }
    [[nodiscard]] bool isFXAAEnabled() const { return fxaaEnabled; }

private:
    unsigned int hdrFBO;
    unsigned int hdrColorBuffer;
    unsigned int rbo;
    unsigned int quadVAO, quadVBO;
    bool fxaaEnabled;

    bool enabled;
    int tonemapAlgorithm;
    float exposure;
    float gamma;

    unsigned int screenWidth, screenHeight;

    void initFramebuffer();
    void initQuad();
    void cleanup();
};

#endif //MY_FIRST_ENGINE_HDRSYSTEM_H