//
// Created by gllekk on 10/10/25.
//

#ifndef MY_FIRST_ENGINE_TIMECLASS_H
#define MY_FIRST_ENGINE_TIMECLASS_H

#include <chrono>
#include <GLFW/glfw3.h>

class TimeManager {
public:
    TimeManager();

    void beginFrame();
    void endFrame();

    float getDeltaTime() const;
    float getFrameTimeMs() const;
    unsigned int getFps() const;

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
