#include "Core/GLDebug.hpp"
#include "Core/Window.hpp"
#include "GUI/WindowSystem.hpp"
#include "Graphics/Model.hpp"
#include "Graphics/Renderer.hpp"
#include "Tools/ModelLoader.hpp"
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

class ViewerApp {
  public:
    ViewerApp() = default;
    ~ViewerApp() = default;

    void Run(const std::string& initialModelPath);
};

inline void ViewerApp::Run(const std::string& initialModelPath) {
    std::string modelPath = initialModelPath;
    Window::InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "3D Model Viewer");
    Window::SetVsync(true);
    // GLDebug::EnableDebugMode();
    OpenglData::SetFaceCulling(false);
    OpenglData::SetDepthTesting(true);
    WindowSystem windowsSystem;

    ModelLoader modelLoader;

    while (!Window::WindowShouldClose()) {
        auto inputData = windowsSystem.GetInputData();
        if (modelPath.size() > 0) {
            WindowSystem::s_modelPath = std::filesystem::path(initialModelPath);
            modelPath.clear();
        }

        Core::OnRenderStart();
        Core::StartRenderingToTexture(windowsSystem.GetViewportWinSize());
        CameraSystem::GetInstance().UpdateInput();
        CameraSystem::GetInstance().SetInputState(inputData.GetAllowCameraInput());
        modelLoader.LoadSelectedModel();

        modelLoader.RenderSelectedModel(inputData);
        // Renderer::GetInstance().RenderSkybox();
        Renderer::GetInstance().SetLightShaderActive(inputData.GetIsLightShaderActive());
        Renderer::GetInstance().SetLightIntensity(inputData.GetLightIntensity());
        Renderer::GetInstance().SetWireframeMode(inputData.GetWireframeMode());
        windowsSystem.ApplyGuiData();
        PerfData::CollectPerformanceData();
        Core::FinishRenderingToTexture();
        windowsSystem.RenderWindows(modelLoader.IsModelLoaded());
        Core::OnRenderEnd();
    }
    Window::DestroyWindow();
}
