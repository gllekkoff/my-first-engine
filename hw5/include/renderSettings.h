//
// Created by gllekk on 12/11/25.
//

#ifndef MY_FIRST_ENGINE_RENDERSETTINGS_H
#define MY_FIRST_ENGINE_RENDERSETTINGS_H

class RenderSettings {
public:
    float shadowBias = 0.005f;
    bool usePCF = false;

    void renderImGui();
};

#endif //MY_FIRST_ENGINE_RENDERSETTINGS_H