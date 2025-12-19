//
// Created by gllekk on 12/11/25.
//
#include "renderSettings.h"
#include <imgui.h>

void RenderSettings::renderImGui() {
    if (ImGui::CollapsingHeader("Shadow Settings")) {
        ImGui::SliderFloat("Shadow Bias", &shadowBias, 0.001f, 0.05f);
        ImGui::Checkbox("Use PCF", &usePCF);
    }
}