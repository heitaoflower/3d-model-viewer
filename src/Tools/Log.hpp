#pragma once
#include <cstdint>
#include <string>
#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif
#include <fstream>
#include <iostream>

class Log
{
  private:
    uint32_t errorCount, warningCount;

  public:
    template<typename... Args>
    static void Error(Args... args) {
        (std::cout << ... << args) << std::endl;
    }

    template<typename... Args>
    static void Warn(Args... args) {
        (std::cout << ... << args) << std::endl;
    }

    template<typename... Args>
    static void Info(Args... args) {
        (std::cout << ... << args) << std::endl;
    }
};
