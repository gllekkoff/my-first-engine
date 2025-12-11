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
    PerformanceLogger(float sampleIntervalSeconds = 1.0f) :
        m_SampleInterval(sampleIntervalSeconds),
        m_SampleTimer(0.0f),
        m_TotalElapsedTime(0.0f)
    {}

    void update(float deltaTime, float currentFrameTimeMs) {
        m_TotalElapsedTime += deltaTime;
        m_SampleTimer += deltaTime;

        if (m_SampleTimer >= m_SampleInterval) {
            m_Samples.push_back({m_TotalElapsedTime, currentFrameTimeMs});
            m_SampleTimer -= m_SampleInterval;
        }
    }

    bool writeToFile(const std::string& filename) const {
        std::ofstream outFile(filename);
        if (!outFile.is_open()) {
            std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
            return false;
        }

        outFile << "Time (s),FrameTime (ms)\n";

        for (const auto& sample : m_Samples) {
            outFile << sample.elapsedTime << "," << sample.frameTime << "\n";
        }

        outFile.close();
        std::cout << "Performance data successfully written to " << filename << std::endl;
        return true;
    }

private:
    float m_SampleInterval;
    float m_SampleTimer;
    float m_TotalElapsedTime;
    std::vector<PerformanceSample> m_Samples;
};

#endif //MY_FIRST_ENGINE_PERFORMANCELOG_H