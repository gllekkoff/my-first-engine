//
// Created by gllekk on 10/10/25.
//

#include "timeClass.h"

TimeManager::TimeManager()
    : m_FrameTimeMs(0.0f),
      m_DeltaTime(0.0f),
      m_LastFrameTime(0.0f),
      m_Fps(0),
      m_FrameCount(0),
      m_FpsTimer(0.0f) {}

void TimeManager::beginFrame() {
    m_StartTime = std::chrono::high_resolution_clock::now();
    const auto currentFrame = static_cast<float>(glfwGetTime());
    m_DeltaTime = currentFrame - m_LastFrameTime;
    m_LastFrameTime = currentFrame;
}

void TimeManager::endFrame() {
    const auto endTime = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - m_StartTime);
    m_FrameTimeMs = duration.count() / 1000.0f;

    m_FrameCount++;
    m_FpsTimer += m_DeltaTime;

    if (m_FpsTimer >= 1.0f) {
        m_Fps = m_FrameCount;
        m_FrameCount = 0;
        m_FpsTimer -= 1.0f;
    }
}

float TimeManager::getDeltaTime() const {
    return m_DeltaTime;
}

float TimeManager::getFrameTimeMs() const {
    return m_FrameTimeMs;
}

unsigned int TimeManager::getFps() const {
    return m_Fps;
}
