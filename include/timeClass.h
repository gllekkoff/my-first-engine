//
// Created by gllekk on 10/10/25.
//

#ifndef MY_FIRST_ENGINE_TIMECLASS_H
#define MY_FIRST_ENGINE_TIMECLASS_H


#include <chrono>
#include <GLFW/glfw3.h>

class TimeManager {
public:
    TimeManager() :
        m_FrameTimeMs(0.0f),
        m_DeltaTime(0.0f),
        m_LastFrameTime(0.0f),
        m_Fps(0),
        m_FrameCount(0),
        m_FpsTimer(0.0f)
    {}

    void beginFrame() {
        m_StartTime = std::chrono::high_resolution_clock::now();

        const auto currentFrame = static_cast<float>(glfwGetTime());
        m_DeltaTime = currentFrame - m_LastFrameTime;
        m_LastFrameTime = currentFrame;
    }

    void endFrame() {
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

    float getDeltaTime() const {
        return m_DeltaTime;
    }

    float getFrameTimeMs() const {
        return m_FrameTimeMs;
    }

    unsigned int getFps() const {
        return m_Fps;
    }

private:
    std::chrono::high_resolution_clock::time_point m_StartTime;
    float m_FrameTimeMs;

    float m_DeltaTime;
    float m_LastFrameTime;

    unsigned int m_Fps;
    unsigned int m_FrameCount;
    float m_FpsTimer;
};

#endif //MY_FIRST_ENGINE_TIMECLASS_H