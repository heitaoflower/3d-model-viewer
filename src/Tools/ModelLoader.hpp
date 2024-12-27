#pragma once
#include "../Graphics/Model.hpp"
#include "../GUI/WindowSystem.hpp"
#include "../Graphics/Renderer.hpp"
#include <filesystem>

enum class USER_INPUT : uint8_t {
    TEXTURE_SELECTION,
    COLOR_SELECTION,
    NO_INPUT
};

class ModelLoader {
  public:
    ModelLoader();
    ~ModelLoader();
    void LoadSelectedModel();
    void RenderSelectedModel(InputData inputData);
    bool IsModelLoaded();
    [[nodiscard]] const std::vector<std::string> FindTexture(const std::string& modelPath) const;

  private:
    MaterialSelection m_materialSelection;
    std::unique_ptr<Model> m_model;
    uint32_t m_diffuseTexture;
    uint32_t m_specularTexture;
    USER_INPUT m_waitingForUserInput = USER_INPUT::NO_INPUT;
    std::vector<std::string> m_texturePaths;
};
