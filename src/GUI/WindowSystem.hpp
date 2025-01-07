#pragma once
#include "../Core/Core.hpp"
#include "../Core/Window.hpp"
#include "../Tools/FileDialog.hpp"
#include "GuiData.hpp"
#include "ImGuizmo.h"
#include "imgui.h"
#include <cstdint>
#include <filesystem>
#include <optional>
#include <vector>
#include "../constants.hpp"


struct MaterialSelection {
    std::optional<std::string> diffuse;
    std::optional<std::string> specular;

    MaterialSelection(std::optional<std::string> diffuse, std::optional<std::string> specular);
    MaterialSelection();
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

    static bool RenderTexturesDialog(MaterialSelection& materialSelection,
                                     std::vector<std::string>& textures);

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
