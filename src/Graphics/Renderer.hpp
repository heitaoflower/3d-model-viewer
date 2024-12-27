#pragma once
#include "../Core/CameraSystem.hpp"
#include "../GUI/WindowSystem.hpp"
#include "Model.hpp"
#include "Skybox.hpp"

enum class Shaders : uint8_t {
    SIMPLE,
    LIGHT,
    REFLECT
};

class Renderer {
  public:
    void RenderModel(const Model& model, const InputData& inputData);
    void RenderSkybox();
    static Renderer& GetInstance();
    [[nodiscard]] uint32_t GetVerticesCount() const;
    [[nodiscard]] uint32_t GetIndicesCount() const;
    void ReloadShaders();
    Shader& GetShader();
    void SetActiveShader(Shaders active);
    void SetLightIntensity(float intensity);
    void SetWireframeMode(bool active);

  private:
    Shaders m_activeShader;

    uint32_t m_verticesCount;
    uint32_t m_indicesCount;
    float m_lightIntensity;
    bool m_wireframeMode;

    Skybox m_skybox;
    Shader m_reflect;
    Shader m_lightShader;
    Shader m_simpleShader;
    Renderer();
    ~Renderer() = default;
};
