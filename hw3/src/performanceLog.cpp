//
// Created by gllekk on 10/10/25.
//

#include "performanceLog.h"

PerformanceLogger::PerformanceLogger(float sampleIntervalSeconds)
    : m_SampleInterval(sampleIntervalSeconds),
      m_SampleTimer(0.0f),
      m_TotalElapsedTime(0.0f) {}

void PerformanceLogger::update(float deltaTime, float currentFrameTimeMs) {
    m_TotalElapsedTime += deltaTime;
    m_SampleTimer += deltaTime;

    if (m_SampleTimer >= m_SampleInterval) {
        m_Samples.push_back({m_TotalElapsedTime, currentFrameTimeMs});
        m_SampleTimer -= m_SampleInterval;
    }
}

bool PerformanceLogger::writeToFile(const std::string& filename) const {
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
