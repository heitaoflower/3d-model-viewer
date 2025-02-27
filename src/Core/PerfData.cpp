﻿#include "PerfData.hpp"

float64 PerfData::s_currentFrame = 0;
float64 PerfData::s_fps = 0;
float64 PerfData::s_deltaTime = 0;
float64 PerfData::s_lastFrame = 0;

float64 PerfData::s_frameCount = 0;
float64 PerfData::s_previousTime = 0;

void PerfData::CollectPerformanceData() {
    s_currentFrame = glfwGetTime();
    s_deltaTime = s_currentFrame - s_lastFrame;
    s_lastFrame = s_currentFrame;

    int framesBuffer = -1;
    s_frameCount++;

    if (s_currentFrame - s_previousTime >= 1.0) {
        framesBuffer = s_frameCount;
        s_frameCount = 0;
        s_previousTime = s_currentFrame;

        s_fps = framesBuffer;
    }
}

float64 PerfData::GetFPS() {
    return s_fps;
}

float64 PerfData::GetDeltaTime() {
    return s_deltaTime;
}
