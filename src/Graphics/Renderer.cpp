#include "Renderer.hpp"

Renderer& Renderer::GetInstance() {
    static Renderer instance;
    return instance;
}

void Renderer::RenderModel(const Model& model, const InputData& inputData) {
    auto modelMatrix = inputData.GetModelMatrix();

    m_indicesCount = model.GetIndicesCount();
    m_verticesCount = model.GetVerticesCount();
    if (m_lightShaderIsActive) {
        m_lightShader.Bind();
        m_lightShader.SetUniform("projection", CameraSystem::GetInstance().GetProjectionMatrix());
        m_lightShader.SetUniform("view", CameraSystem::GetInstance().GetViewMatrix());
        m_lightShader.SetUniform("model", modelMatrix);
        m_lightShader.SetUniform("intensityMultiplier", m_lightIntensity);
        m_lightShader.SetUniform("color", model.GetColor());
        m_lightShader.SetUniform("lightPos", inputData.GetLightPos());
        m_lightShader.SetUniform("shininess", inputData.GetMaterialShininess());
    }
    else {
        m_simpleShader.Bind();
        m_simpleShader.SetUniform("projection", CameraSystem::GetInstance().GetProjectionMatrix());
        m_simpleShader.SetUniform("view", CameraSystem::GetInstance().GetViewMatrix());
        m_simpleShader.SetUniform("model", modelMatrix);
        m_simpleShader.SetUniform("color", model.GetColor());
    }

    model.DrawArrays();
}

void Renderer::RenderSkybox() {
    glm::mat4 view = glm::mat4(glm::mat3(CameraSystem::GetInstance().GetViewMatrix()));
    glm::mat4 projection = glm::perspective(
        glm::radians(CameraSystem::GetInstance().GetFov()), 16.f / 9.f, 0.1f, 150.0f);

    glDepthFunc(GL_LEQUAL);
    m_skybox.GetSkyboxShader().SetUniform("view", view);
    m_skybox.GetSkyboxShader().SetUniform("projection", projection);
    m_skybox.GetSkyboxShader().Bind();
    uint32_t vao = m_skybox.GetVAO();
    glBindVertexArray(vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_skybox.GetCubeMapTex());
    OpenglAPI::DrawArrays(GL_TRIANGLES, 36);
    glDepthFunc(GL_LESS);
    glBindVertexArray(0);
}

uint32_t Renderer::GetVerticesCount() const {
    return m_verticesCount;
}

uint32_t Renderer::GetIndicesCount() const {
    return m_indicesCount;
}

void Renderer::SetLightShaderActive(bool active) {
    m_lightShaderIsActive = active;
}

void Renderer::SetLightIntensity(float intensity) {
    m_lightIntensity = intensity;
}

void Renderer::SetWireframeMode(bool active) {
    m_wireframeMode = active;
    glPolygonMode(GL_FRONT_AND_BACK, m_wireframeMode ? GL_LINE : GL_FILL);
}

Renderer::Renderer()
    : m_verticesCount(0)
    , m_indicesCount(0)
    , m_lightShaderIsActive(true)
    , m_lightIntensity(1.0f)
    , m_wireframeMode(false)
    , m_skybox("/home/dominik/Projekty/3d-model-viewer/res/skybox", ".png")
    , m_lightShader(
          (std::string(GLOBAL_PATH) + "/home/dominik/Projekty/3d-model-viewer/res/light.frag.glsl")
              .c_str(),
          (std::string(GLOBAL_PATH) + "/home/dominik/Projekty/3d-model-viewer/res/light.vert.glsl")
              .c_str())
    , m_simpleShader(
          (std::string(GLOBAL_PATH) + "/home/dominik/Projekty/3d-model-viewer/res/simple.frag.glsl")
              .c_str(),
          (std::string(GLOBAL_PATH) + "/home/dominik/Projekty/3d-model-viewer/res/simple.vert.glsl")
              .c_str()) {}

Shader& Renderer::GetShader() {
    return m_lightShader;
}
