﻿#pragma once
#include "Keyboard.hpp"
#include "glm/glm.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <optional>

class ArcBallCamera {
  public:
    ArcBallCamera();
    ~ArcBallCamera() = default;
    void ProcessMouseInput(float definedXPos, float definedYPos);
    void RotateAzimuth(const float radians);
    void RotatePolar(const float radians);
    void Zoom(const float by);

    void MoveHorizontal(const float distance);
    void MoveVertical(const float distance);

    void SetFov(const float fov);
    void SetNear(const float near);
    void SetFar(const float far);

    void SetProjMatToOrtho();
    void SetProjMatToPerspective();

    const glm::mat4 GetProjecionMatrix(const glm::vec2 viewportSize);
    [[nodiscard]] const glm::mat4 GetViewMatrix() const;
    [[nodiscard]] float GetFov() const;
    [[nodiscard]] float GetNear() const;
    [[nodiscard]] float GetFar() const;

  private:
    void RecreateProjectionMatrix(const glm::vec2 viewportSize);

    bool m_isOrtho;
    float m_fov;
    float m_near;
    float m_far;
    std::optional<glm::mat4> m_projectionMatrix; // Ortho / perspective
    float m_sensitivity;

    glm::vec3 m_center;
    glm::vec3 m_upVector;
    float m_radius;
    float m_minRadius;

    float m_azimuthAngle;
    float m_polarAngle;

    bool m_firstMouse;
    float m_lastXPos;
    float m_lastYPos;
    float m_rotationSpeed;
};
