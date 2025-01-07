#pragma once
#include "../Core/CameraSystem.hpp"
#include "../GUI/WindowSystem.hpp"
#include "Model.hpp"
#include "ShaderManager.hpp"
#include "Skybox.hpp"

class Renderer {
  public:
    void RenderModel(const Model& model, const InputData& inputData);
    void RenderAdditional();
    static Renderer& GetInstance();
    [[nodiscard]] uint32_t GetVerticesCount() const;
    [[nodiscard]] uint32_t GetIndicesCount() const;
    void ReloadShaders();
    Shader& GetShader();
    void SetActiveShader(ShaderType active);
    void SetLightIntensity(float intensity);
    void SetWireframeMode(bool active);

  private:
    ShaderType m_activeShader;

    uint32_t m_verticesCount;
    uint32_t m_indicesCount;
    float m_lightIntensity;
    bool m_wireframeMode;

    Skybox m_skybox;
    ShaderBatch m_shaderBatch;
    Renderer();
    ~Renderer() = default;
};
