#pragma once
#include "../Graphics/Model.hpp"
#include "../GUI/WindowSystem.hpp"
#include "../Graphics/Renderer.hpp"
#include <filesystem>

enum USER_INPUT {
    TEXTURE_SELECTION,
    COLOR_SELECTION,
    NO_INPUT
};

enum MODEL_EXPORT_TYPE {
    OBJ,
    GLTF,
    FBX,
};

std::string getExportTypeString(MODEL_EXPORT_TYPE exportType);

class ModelLoader {
  private:
    std::unique_ptr<Model> m_model;
    uint32_t m_texture;
    USER_INPUT m_waitingForUserInput = NO_INPUT;
    std::vector<std::string> m_texturePaths;
    std::optional<MODEL_EXPORT_TYPE> m_lastExportType;

  public:
    ModelLoader();
    ~ModelLoader();
    void LoadSelectedModel();
    void RenderSelectedModel(InputData inputData);
    bool IsModelLoaded();
    const std::vector<std::string> FindTexture(const std::string modelPath) const;
};
