﻿#pragma once
#include "../Core/CameraSystem.hpp"
#include "../GUI/WindowSystem.hpp"
#include "Model.hpp"

class Renderer
{
  public:
    const void RenderModel(const Model &model, const InputData &inputData);
    static Renderer &GetInstance();
    const uint32_t GetVerticesCount() const;
    const uint32_t GetIndicesCount() const;
    void ReloadShaders();
    Shader &GetShader();
    void SetLightShaderActive(bool active);
    void SetLightIntensity(float intensity);
    void SetWireframeMode(bool active);
  private:
    uint32_t m_verticesCount;
    uint32_t m_indicesCount;
    bool m_lightShaderIsActive;
    float m_lightIntensity;
    bool m_wireframeMode;

    Shader m_lightShader;
    Shader m_simpleShader;
    Renderer();
    ~Renderer() = default;
};
