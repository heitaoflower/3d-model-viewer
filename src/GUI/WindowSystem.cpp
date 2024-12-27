#include "WindowSystem.hpp"
#include "../Graphics/Renderer.hpp"
#include "ImGuizmo.h"
#include "imgui.h"
#include "GuiStyle.hpp"
#include <ranges>
#include <utility>

glm::vec2 WindowSystem::s_viewportWinSize = glm::vec2(-1.0f);
std::optional<std::filesystem::path> WindowSystem::s_modelPath = std::nullopt;
bool WindowSystem::s_flipTexture = false;
bool WindowSystem::s_showTextureErrorWindow = false;
bool WindowSystem::s_showModelErrorWindow = false;

InputData::InputData(glm::vec3 modelPos,
                     glm::vec3 modelRot,
                     glm::vec3 modelScale,
                     float modelRotAngle,
                     bool allowCameraInput)
    : m_modelPos(modelPos)
    , m_modelRot(modelRot)
    , m_modelScale(modelScale)
    , m_modelRotAngle(modelRotAngle)
    , m_allowCameraInput(allowCameraInput)
    , m_lightShaderActive(true)
    , m_simpleShaderActive(false)
    , m_reflectShaderActive(false)
    , m_lightIntensity(0.5f)
    , m_wireframeMode(false)
    , m_materialShininess(32.0f)
    , m_lightPos(1.2f, 1.0f, 2.0f)
    , m_skyboxActive(true) {
    model = glm::mat4(1.0f);
    model = glm::translate(model, modelPos);
    model = glm::rotate(model, glm::radians(modelRotAngle), modelRot);
    model = glm::scale(model, modelScale);
}

bool InputData::GetSkyboxActive() const {
    return m_skyboxActive;
}

bool InputData::GetAllowCameraInput() const {
    return m_allowCameraInput;
}

Shaders InputData::GetActiveShader() const {
    if (m_lightShaderActive)
        return Shaders::LIGHT;
    else if (m_simpleShaderActive)
        return Shaders::SIMPLE;
    else
        return Shaders::REFLECT;
}

float InputData::GetLightIntensity() const {
    return m_lightIntensity;
}

bool InputData::GetWireframeMode() const {
    return m_wireframeMode;
}

glm::vec3 InputData::GetLightPos() const {
    return m_lightPos;
}

float InputData::GetMaterialShininess() const {
    return m_materialShininess;
}

const glm::mat4 InputData::GetModelMatrix() const {
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, -1.0f, 1.0f));

    return scaleMatrix * model;
}

CameraSettings::CameraSettings() noexcept
    : cameraType(0)
    , projectionType(2)
    , zoomMultiplier(1.f) {}

WindowSystem::WindowSystem()
    : m_inputData(glm::vec3(2.f, 0.f, -5.f), glm::vec3(1.0f), glm::vec3(1.0f), 0.0f, true)
    , m_gizmoOperation(ImGuizmo::TRANSLATE)
    , m_renderGizmo(false)
    , m_gizmoSizeMultiplier(1.0f) {

    SetGuiStyle();
    s_viewportWinSize = glm::vec2(uint32_t(500), uint32_t(500));
}

void WindowSystem::RenderWindows(bool isObjectRendered) {
    ImVec2 mainMenuBarSize = this->RenderMainMenuBar(isObjectRendered);
    ImVec2 screenSize = ImGui::GetIO().DisplaySize;
    ImGui::Begin("Viewport",
                 nullptr,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
                     | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
                     | ImGuiWindowFlags_NoBringToFrontOnFocus);
    ImGui::SetWindowSize(ImVec2(screenSize.x / 1.5, screenSize.y - mainMenuBarSize.y));
    ImGui::SetWindowPos(ImVec2(0, mainMenuBarSize.y));

    auto winSize = ImGui::GetWindowSize(); // Velikost okna viewportu (ImGui okna)

    winSize.y -= 40;
    if (winSize.x > 0 && winSize.y > 0)
        s_viewportWinSize = glm::vec2((uint16_t)winSize.x, (uint16_t)winSize.y);

    if (isObjectRendered)
        ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(Core::GetRenderTargetTexture())),
                     ImVec2(s_viewportWinSize.x, s_viewportWinSize.y));
    else {
        float windowWidth = ImGui::GetWindowSize().x;
        float windowHeight = ImGui::GetWindowSize().y;

        float textWidth = ImGui::CalcTextSize("Není načtený žádný model").x;
        float textHeight = ImGui::CalcTextSize("Není načtený žádný model").y;

        ImGui::SetCursorPos(
            ImVec2((windowWidth - textWidth) / 2.0f, (windowHeight - textHeight) / 2.0f - 60.f));
        ImGui::Text("Není načtený žádný model");

        textWidth = ImGui::CalcTextSize("Otevřete model pomocí menu Soubor -> Otevřít").x;
        textHeight = ImGui::CalcTextSize("Otevřete model pomocí menu Soubor -> Otevřít").y;

        ImGui::SetCursorPos(
            ImVec2((windowWidth - textWidth) / 2.0f, (windowHeight - textHeight) / 2.0f - 30.f));
        ImGui::Text("Otevřete model pomocí menu Soubor -> Otevřít");

        ImGui::Dummy(ImVec2(0, 10));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0, 10));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(30, 10));
        ImGui::SetCursorPosX((windowWidth - 120) / 2.0f);
        if (ImGui::Button("Načíst model", ImVec2(150, 50))) {
            OpenModelSelectionDialog();
        }
        ImGui::PopStyleVar();
    }
    this->m_inputData.m_allowCameraInput
        = (ImGui::IsWindowHovered() || !Window::GetCursorVisibility()) & !m_renderGizmo;

    // Gizmo
    if (isObjectRendered && m_cameraSettings.cameraType != 1 && m_renderGizmo) {
        ImGuizmo::BeginFrame();
        ImGuizmo::Enable(true);
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();
        ImGuizmo::SetGizmoSizeClipSpace(0.15f * m_gizmoSizeMultiplier);
        ImGuizmo::SetRect(0, 0, s_viewportWinSize.x, s_viewportWinSize.y);
        ImGuizmo::Manipulate(glm::value_ptr(CameraSystem::GetInstance().GetViewMatrix()),
                             glm::value_ptr(CameraSystem::GetInstance().GetProjectionMatrix()),
                             m_gizmoOperation,
                             ImGuizmo::WORLD,
                             glm::value_ptr(m_inputData.model));
    }
    // --------------

    ImGui::End();

    ImGui::Begin("Vlastnosti",
                 nullptr,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
                     | ImGuiWindowFlags_NoBringToFrontOnFocus);

    ImGui::SetWindowSize(
        ImVec2(screenSize.x - screenSize.x / 1.5, screenSize.y - mainMenuBarSize.y));
    ImGui::SetWindowPos(ImVec2(screenSize.x / 1.5, mainMenuBarSize.y));

    if (!isObjectRendered)
        ImGui::BeginDisabled();

    RenderClearColorPicker();
    RenderModelInfo();
    ImGui::Checkbox("Wireframe", &m_inputData.m_wireframeMode);
    ImGui::Separator();
    if (m_renderGizmo)
        ImGui::BeginDisabled();
    RenderCameraSettings();
    if (m_renderGizmo)
        ImGui::EndDisabled();

    RenderGizmoSettings();
    RenderShaderSettings();

    if (!isObjectRendered)
        ImGui::EndDisabled();

    RenderModelErrorWindow();
    RenderTextureErrorWindow();
    ImGui::End();
}

void WindowSystem::OpenModelSelectionDialog() {
    auto path = FileDialogManager::GetInstance().InvokeFileDialog();

    std::filesystem::path modelPath = path;
    std::string extension = modelPath.extension().string();

    if (modelPath.empty()) {
        Log::Info("Nebyl vybraný žádný soubor");
        return;
    }

    if (std::find(SUPPORTED_MODEL_EXTENSIONS.begin(), SUPPORTED_MODEL_EXTENSIONS.end(), extension)
        != SUPPORTED_MODEL_EXTENSIONS.end()) {
        s_modelPath = path;
    }
    else {
        s_showModelErrorWindow = true;
        Log::Info("Nebyl vybraný správný 3D model");
    }
}

void WindowSystem::RenderModelErrorWindow() {
    if (s_showModelErrorWindow) {
        ImGui::OpenPopup("Nebyl vybraný 3D model");
        if (ImGui::BeginPopupModal("Nebyl vybraný 3D model",
                                   nullptr,
                                   ImGuiWindowFlags_AlwaysAutoResize
                                       | ImGuiWindowFlags_NoCollapse)) {
            ImGui::Text("Vybraný soubor nemá známou příponu 3D modelu\n\n");
            ImGui::Separator();

            if (ImGui::Button("OK", ImVec2(120, 0))) {
                s_showModelErrorWindow = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }
}

void WindowSystem::RenderTextureErrorWindow() {
    if (s_showTextureErrorWindow) {
        ImGui::OpenPopup("Nebyla vybráný podporovaný formát textury");
        if (ImGui::BeginPopupModal("Nebyla vybráný podporovaný formát textury",
                                   nullptr,
                                   ImGuiWindowFlags_AlwaysAutoResize
                                       | ImGuiWindowFlags_NoCollapse)) {
            ImGui::Text("Vybraný soubor nemá známou/podporovanou příponu textury\n\n");
            ImGui::Separator();

            if (ImGui::Button("OK", ImVec2(120, 0))) {
                s_showTextureErrorWindow = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
}

void WindowSystem::RenderShaderSettings() {
    ImGui::Text("Nastavení shaderu");
    int shaderType = 0;
    if (m_inputData.m_simpleShaderActive)
        shaderType = 0;
    if (m_inputData.m_lightShaderActive)
        shaderType = 1;
    if (m_inputData.m_reflectShaderActive)
        shaderType = 2;

    if (ImGui::RadioButton("Jednoduchý shader", shaderType == 0)) {
        shaderType = 0;
    }
    if (ImGui::RadioButton("Light shader", shaderType == 1)) {
        shaderType = 1;
    }

    if (!m_inputData.m_skyboxActive)
        ImGui::BeginDisabled();

    if (ImGui::RadioButton("Reflect shader", shaderType == 2)) {
        shaderType = 2;
    }

    if (!m_inputData.m_skyboxActive)
        ImGui::EndDisabled();

    m_inputData.m_simpleShaderActive = (shaderType == 0);
    m_inputData.m_lightShaderActive = (shaderType == 1);
    m_inputData.m_reflectShaderActive = (shaderType == 2);

    if (m_inputData.m_reflectShaderActive) {
        ImGui::BeginDisabled();
    }

    ImGui::Checkbox("Zobrazit skybox", &m_inputData.m_skyboxActive);

    if (m_inputData.m_reflectShaderActive) {
        ImGui::EndDisabled();
    }

    if (!m_inputData.m_lightShaderActive) {
        ImGui::BeginDisabled();
    }

    ImGui::SliderFloat("Intenzita světla", &m_inputData.m_lightIntensity, 0.0f, 1.0f);
    ImGui::SliderFloat("Lesklost materiálu", &m_inputData.m_materialShininess, 0.0f, 256.0f);
    ImGui::DragFloat3("Pozice světla", glm::value_ptr(m_inputData.m_lightPos), 1.0f);
    if (!m_inputData.m_lightShaderActive) {
        ImGui::EndDisabled();
    }

    ImGui::Separator();
}

ImVec2 WindowSystem::RenderMainMenuBar(bool isObjectRendered) {
    ImVec2 mainMenuBarSize = ImVec2(0, 0);

    if (ImGui::BeginMainMenuBar()) {
        mainMenuBarSize = ImGui::GetWindowSize();

        if (ImGui::BeginMenu("Soubor")) {
            if (ImGui::MenuItem("Otevřít")) {
                WindowSystem::OpenModelSelectionDialog();
            }

            if (ImGui::MenuItem("Zavřít")) {
                exit(0);
            }
            ImGui::EndMenu();
        }

        if (!isObjectRendered)
            ImGui::BeginDisabled();

        if (!isObjectRendered)
            ImGui::EndDisabled();

        if (m_cameraSettings.cameraType == 1) {
            ImGui::Separator();
            ImGui::Text("Pohyb kamery pomocí WASD");
            ImGui::Text("Pro odemčení kurzoru stiskněte F2, pro zamčení F1");
        }

        ImGui::EndMainMenuBar();
    }
    return mainMenuBarSize;
}

void WindowSystem::RenderClearColorPicker() {
    static ImVec4 color = ImVec4(1.f, 1.f, 1.f, 1.f);
    ImGui::ColorEdit3("Barva pozadí", (float*)&color);
    Core::SetBackgroundColor(glm::vec3(color.x, color.y, color.z));
}

void WindowSystem::RenderModelInfo() {
    ImGui::Separator();
    ImGui::Text("Model info");
    ImGui::Text("Počet vertexů: %d", Renderer::GetInstance().GetVerticesCount());
    ImGui::Text("Počet indexů: %d", Renderer::GetInstance().GetIndicesCount());
    ImGui::Separator();
}

MaterialSelection::MaterialSelection(std::optional<std::string> diffuse,
                                     std::optional<std::string> specular) {
    this->diffuse = std::move(diffuse);
    this->specular = std::move(specular);
}

MaterialSelection::MaterialSelection()
    : diffuse(std::nullopt)
    , specular(std::nullopt) {}

bool WindowSystem::RenderTexturesDialog(MaterialSelection& materialSelection,
                                        std::vector<std::string>& textures) {
    if (s_showTextureErrorWindow) {
        return false;
    }

    ImGui::Begin("Automatické hledání textur",
                 nullptr,
                 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
    ImGui::Text("Vyberte diffuse texturu");

    for (auto& texture : textures) {
        bool isSelected = (materialSelection.diffuse == texture);
        if (ImGui::Selectable((texture + " -> Diffuse").c_str(), isSelected)) {
            materialSelection.diffuse = texture;
            Log::Info("Diffuse textura: " + texture);
        }
        if (isSelected) {
            ImGui::SetItemDefaultFocus();
        }
    }

    ImGui::Separator();
    ImGui::Text("Vyberte specular texturu");

    for (auto& texture : textures) {
        bool isSelected = (materialSelection.specular == texture);
        if (ImGui::Selectable((texture + " -> Specular").c_str(), isSelected)) {
            materialSelection.specular = texture;
            Log::Info("Specular textura: " + texture);
        }
        if (isSelected) {
            ImGui::SetItemDefaultFocus();
        }
    }

    ImGui::Text("Nebo vyberte texturu ručně");
    ImGui::Checkbox("Obrátit texturu", &s_flipTexture);
    if (ImGui::Button("Vybrat texturu")) {
        auto texture = FileDialogManager::GetInstance().InvokeFileDialog();
        std::string extension = std::filesystem::path(texture).extension().string();
        if (std::find(
                SUPPORTED_TEXTURE_EXTENSIONS.begin(), SUPPORTED_TEXTURE_EXTENSIONS.end(), extension)
            == SUPPORTED_TEXTURE_EXTENSIONS.end()) {
            Log::Error("Nepodporovaný formát textury:" + texture);
            s_showTextureErrorWindow = true;
            ImGui::End();
            return false;
        }
        else {
            materialSelection.diffuse = texture;
        }
    }

    ImGui::Separator();
    ImGui::Text("Nebo vyberte barvu modelu");

    if (ImGui::Button("Vybrat barvu")) {
        materialSelection.diffuse = std::string("");
    }

    ImGui::Separator();
    if (ImGui::Button("Potvrdit")) {
        ImGui::End();
        if (!materialSelection.diffuse.has_value()) {
            return false;
        }
        return true;
    }
    ImGui::End();

    return false;
}

const std::optional<glm::vec3> WindowSystem::RenderModelColorPicker() {
    ImGui::Begin(
        "Barva modelu", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
    static ImVec4 color = ImVec4(1.f, 1.f, 1.f, 1.f);
    ImGui::ColorEdit3("Barva modelu", (float*)&color);

    if (ImGui::Button("Potvrdit")) {
        ImGui::End();
        return glm::vec3(color.x, color.y, color.z);
    }
    ImGui::End();
    return std::nullopt;
}

void WindowSystem::RenderGizmoSettings() {
    ImGui::Text("Nastavení gizma");
    ImGui::Checkbox("Zobrazit gizmo", &m_renderGizmo);
    if (!m_renderGizmo) {
        ImGui::BeginDisabled();
    }

    ImGui::SliderFloat("Velikost gizma", &m_gizmoSizeMultiplier, 0.5, 1.5);
    if (ImGui::RadioButton("Přesun", m_gizmoOperation == ImGuizmo::TRANSLATE))
        m_gizmoOperation = ImGuizmo::TRANSLATE;
    if (ImGui::RadioButton("Rotace", m_gizmoOperation == ImGuizmo::ROTATE))
        m_gizmoOperation = ImGuizmo::ROTATE;
    if (ImGui::RadioButton("Škálování", m_gizmoOperation == ImGuizmo::SCALE))
        m_gizmoOperation = ImGuizmo::SCALE;

    if (!m_renderGizmo) {
        ImGui::EndDisabled();
    }

    ImGui::Separator();
}

void WindowSystem::RenderCameraSettings() {
    ImGui::Text("Nastavení kamery");
    if (ImGui::RadioButton("Freefly kamera", m_cameraSettings.cameraType == 1))
        m_cameraSettings.cameraType = 1;
    if (ImGui::RadioButton("Orbitální kamera", m_cameraSettings.cameraType == 0))
        m_cameraSettings.cameraType = 0;
    if (m_cameraSettings.cameraType == 1) {
        ImGui::BeginDisabled();
    }
    ImGui::Separator();
    if (ImGui::RadioButton("Perspektivní projekce", m_cameraSettings.projectionType == 2))
        m_cameraSettings.projectionType = 2;
    if (ImGui::RadioButton("Ortogonální projekce", m_cameraSettings.projectionType == 3))
        m_cameraSettings.projectionType = 3;

    if (m_cameraSettings.projectionType != 2)
        ImGui::BeginDisabled();
    ImGui::SliderFloat("Zoom", &m_cameraSettings.zoomMultiplier, 0.1f, 10.0f);
    if (m_cameraSettings.projectionType != 2)
        ImGui::EndDisabled();

    if (m_cameraSettings.cameraType == 1) {
        ImGui::EndDisabled();
    }

    ImGui::Separator();
}

void WindowSystem::ApplyGuiData() {
    if (m_cameraSettings.projectionType == 2) {
        CameraSystem::GetInstance().SetProjMatToPerspective();
    }
    else if (m_cameraSettings.projectionType == 3) {
        CameraSystem::GetInstance().SetProjMatToOrtho();
    }

    if (m_cameraSettings.cameraType == 0) {
        CameraSystem::GetInstance().SetActiveCamera(Cameras::ARCBALL);
    }
    else if (m_cameraSettings.cameraType == 1) {
        CameraSystem::GetInstance().SetActiveCamera(Cameras::FIRST_PERSON);
    }

    CameraSystem::GetInstance().SetZoomMultiplier(m_cameraSettings.zoomMultiplier);
}

const glm::vec2 WindowSystem::GetViewportWinSize() {
    return s_viewportWinSize;
}

InputData WindowSystem::GetInputData() {
    return m_inputData;
}
