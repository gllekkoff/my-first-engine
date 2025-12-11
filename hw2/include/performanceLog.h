//
// Created by gllekk on 10/10/25.
//

#ifndef MY_FIRST_ENGINE_PERFORMANCELOG_H
#define MY_FIRST_ENGINE_PERFORMANCELOG_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

struct PerformanceSample {
    float elapsedTime;
    float frameTime;
};

class PerformanceLogger {
public:
    PerformanceLogger(float sampleIntervalSeconds = 1.0f);

    void update(float deltaTime, float currentFrameTimeMs);

    bool writeToFile(const std::string& filename) const;

private:
    float m_SampleInterval;
    float m_SampleTimer;
    float m_TotalElapsedTime;
    std::vector<PerformanceSample> m_Samples;
};

#endif //MY_FIRST_ENGINE_PERFORMANCELOG_H