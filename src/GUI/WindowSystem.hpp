#pragma once
#include "../Core/Core.hpp"
#include "../Core/Window.hpp"
#include "../Tools/FileDialog.hpp"
#include "ImGuizmo.h"
#include "imgui.h"
#include <cstdint>
#include <filesystem>
#include <optional>
#include <vector>
#include "../constants.hpp"

struct InputData {
  private:
    glm::vec3 m_modelPos;
    glm::vec3 m_modelRot;
    glm::vec3 m_modelScale;
    float m_modelRotAngle;
    bool m_allowCameraInput;
    bool m_isLightShaderActive;
    float m_lightIntensity;
    bool m_wireframeMode;
    glm::mat4 model;
    float m_materialShininess;
    glm::vec3 m_lightPos;

  public:
    InputData(glm::vec3 modelPos,
              glm::vec3 modelRot,
              glm::vec3 modelScale,
              float modelRotAngle,
              bool allowCameraInput);
    [[nodiscard]] const glm::mat4 GetModelMatrix() const;
    [[nodiscard]] bool GetAllowCameraInput() const;
    [[nodiscard]] bool GetIsLightShaderActive() const;
    [[nodiscard]] float GetLightIntensity() const;
    [[nodiscard]] bool GetWireframeMode() const;
    [[nodiscard]] glm::vec3 GetLightPos() const;
    [[nodiscard]] float GetMaterialShininess() const;
    friend class WindowSystem;
};

struct CameraSettings {
    int cameraType;
    int projectionType;
    float zoomMultiplier;
    CameraSettings() noexcept;
};

class WindowSystem {
  public:
    WindowSystem();
    void RenderWindows(bool isObjectRendered);
    InputData GetInputData();
    void ApplyGuiData();
    static const glm::vec2 GetViewportWinSize();
    static std::optional<std::filesystem::path> s_modelPath;
    static bool s_flipTexture;
    static const std::optional<std::string> RenderTexturesDialog(std::vector<std::string> textures);
    static const std::optional<glm::vec3> RenderModelColorPicker();

  private:
    static glm::vec2 s_viewportWinSize;
    inline void RenderPositionsWidgets();
    inline void RenderClearColorPicker();
    inline void RenderModelInfo();
    inline void RenderCameraSettings();
    inline void RenderModelErrorWindow();
    inline void RenderTextureErrorWindow();
    inline void RenderShaderSettings();
    inline ImVec2 RenderMainMenuBar(bool isObjectRendered);
    inline void OpenModelSelectionDialog();
    inline void RenderGizmoSettings();
    InputData m_inputData;
    CameraSettings m_cameraSettings;
    ImGuizmo::OPERATION m_gizmoOperation;
    bool m_renderGizmo;
    float m_gizmoSizeMultiplier;
    static bool s_showModelErrorWindow;
    static bool s_showTextureErrorWindow;
};
