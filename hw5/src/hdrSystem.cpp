//
// Created by gllekk on 12/19/25.
//

#include "hdrSystem.h"
#include <imgui.h>
#include <iostream>

HDRSystem::HDRSystem(unsigned int width, unsigned int height)
    : hdrFBO(0), hdrColorBuffer(0), rbo(0), quadVAO(0), quadVBO(0),
      enabled(true), fxaaEnabled(true), tonemapAlgorithm(0),
      exposure(1.0f), gamma(2.2f),
      screenWidth(width), screenHeight(height) {
    initFramebuffer();
    initQuad();
}

HDRSystem::~HDRSystem() {
    cleanup();
}

void HDRSystem::initFramebuffer() {
    glGenFramebuffers(1, &hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

    glGenTextures(1, &hdrColorBuffer);
    glBindTexture(GL_TEXTURE_2D, hdrColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdrColorBuffer, 0);

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR: HDR Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void HDRSystem::initQuad() {
    float quadVertices[] = {
        -1.0f,  1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);
}

void HDRSystem::resize(unsigned int width, unsigned int height) {
    if (width == screenWidth && height == screenHeight) return;

    screenWidth = width;
    screenHeight = height;

    cleanup();
    initFramebuffer();
}

void HDRSystem::bindFramebuffer() {
    if (enabled) {
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    }
}

void HDRSystem::unbindFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void HDRSystem::renderToScreen(Shader& hdrShader) {
    if (!enabled) return;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    hdrShader.use();
    hdrShader.setInt("hdrBuffer", 0);
    hdrShader.setInt("tonemapAlgorithm", tonemapAlgorithm);
    hdrShader.setFloat("exposure", exposure);
    hdrShader.setFloat("gamma", gamma);
    hdrShader.setBool("fxaaEnabled", fxaaEnabled);
    hdrShader.setVec2("screenSize", glm::vec2(screenWidth, screenHeight));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrColorBuffer);
    glBindVertexArray(quadVAO);
    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);
    glBindVertexArray(0);
}


void HDRSystem::renderImGui() {
    if (ImGui::CollapsingHeader("HDR & Tonemapping")) {
        ImGui::Checkbox("Enable HDR", &enabled);
        ImGui::Checkbox("Enable FXAA", &fxaaEnabled);

        const char* algorithms[] = {
            "Non-tonemapped HDR",
            "HDR Gamma-corrected",
            "ACES",
            "Uncharted 2",
            "Filmic",
            "Reinhard",
            "Reinhard luma"
        };
        ImGui::Combo("Tonemap Algorithm", &tonemapAlgorithm, algorithms, 7);
        ImGui::SliderFloat("Exposure", &exposure, 0.1f, 5.0f);
        ImGui::SliderFloat("Gamma", &gamma, 1.8f, 2.6f);
    }
}



void HDRSystem::cleanup() {
    if (hdrFBO) glDeleteFramebuffers(1, &hdrFBO);
    if (hdrColorBuffer) glDeleteTextures(1, &hdrColorBuffer);
    if (rbo) glDeleteRenderbuffers(1, &rbo);
}
