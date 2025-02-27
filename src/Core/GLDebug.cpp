﻿#include "GLDebug.hpp"

void GLDebug::EnableDebugMode() {
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // SynchronnĂ­ vĂ˝stup
    glDebugMessageCallback(glDebugOutput, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

    Log::Warn("OpenGL Debug mode je Aktivni!");
}

GLenum GLDebug::CheckForError_(const char* file, int line) {
    GLenum errorCode = glGetError();
    std::string errorMessage;
    while (errorCode != GL_NO_ERROR) {
        switch (errorCode) {
        case GL_INVALID_ENUM:
            errorMessage = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            errorMessage = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            errorMessage = "INVALID_OPERATION";
            break;
        case GL_STACK_OVERFLOW:
            errorMessage = "STACK_OVERFLOW";
            break;
        case GL_STACK_UNDERFLOW:
            errorMessage = "STACK_UNDERFLOW";
            break;
        case GL_OUT_OF_MEMORY:
            errorMessage = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            errorMessage = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        default:
            errorMessage = "UNKNOWN";
            break;
        }
        Log::Error(std::string(file) + " (" + std::to_string(line) + ") " + "-> " + errorMessage
                   + " (" + std::to_string(errorCode) + ")");
    }

    std::cout << '\n';

    return errorCode;
}

void APIENTRY glDebugOutput(GLenum source,
                            GLenum type,
                            uint32_t id,
                            GLenum severity,
                            GLsizei /*length*/,
                            const char* message,
                            const void* /*userParam*/) {

    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
        return;

    std::cout << "---------------" << '\n';
    std::cout << "Debug message (" << id << "): " << message << '\n';

    switch (source) {
    case GL_DEBUG_SOURCE_API:
        std::cout << "Source: API";
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        std::cout << "Source: Window System";
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        std::cout << "Source: Shader Compiler";
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        std::cout << "Source: Third Party";
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        std::cout << "Source: Application";
        break;
    case GL_DEBUG_SOURCE_OTHER:
        std::cout << "Source: Other";
        break;
    default:
        std::cout << "Source: Unknown";
    }
    std::cout << '\n';

    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        std::cout << "Type: Error";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        std::cout << "Type: Deprecated Behaviour";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        std::cout << "Type: Undefined Behaviour";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        std::cout << "Type: Portability";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        std::cout << "Type: Performance";
        break;
    case GL_DEBUG_TYPE_MARKER:
        std::cout << "Type: Marker";
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        std::cout << "Type: Push Group";
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        std::cout << "Type: Pop Group";
        break;
    case GL_DEBUG_TYPE_OTHER:
        std::cout << "Type: Other";
        break;
    default:
        std::cout << "Type: Unknown";
        break;
    }
    std::cout << '\n';

    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        std::cout << "Severity: high";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        std::cout << "Severity: medium";
        break;
    case GL_DEBUG_SEVERITY_LOW:
        std::cout << "Severity: low";
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        std::cout << "Severity: notification";
        break;
    default:
        std::cout << "Severity: unknown";
        break;
    }
    std::cout << '\n';
    std::cout << '\n';
}
