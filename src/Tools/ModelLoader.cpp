#include "ModelLoader.hpp"

ModelLoader::ModelLoader()
    : m_model(nullptr)
    , m_diffuseTexture(0)
    , m_specularTexture(0)
    , m_texturePaths(std::vector<std::string>()) {}

void ModelLoader::LoadSelectedModel() {
    if (WindowSystem::s_modelPath.has_value()) {
        if (m_model != nullptr) {
            m_texturePaths.clear();
            m_model.reset();
            m_model = nullptr;
            m_texturePaths = std::vector<std::string>();
        }

        m_model = std::make_unique<Model>(WindowSystem::s_modelPath.value().string());
        try {
            m_texturePaths = FindTexture(WindowSystem::s_modelPath.value().string());
            m_waitingForUserInput = USER_INPUT::TEXTURE_SELECTION;

            WindowSystem::s_modelPath.reset();
        }
        catch (const std::exception& e) {
            Log::Warn("Model loading failed: " + std::string(e.what()));
            return;
        }
    }
}

void ModelLoader::RenderSelectedModel(InputData inputData) {

    if (m_waitingForUserInput == USER_INPUT::TEXTURE_SELECTION) {
        if (m_diffuseTexture != 0) {
            glDeleteTextures(1, &m_diffuseTexture);
            m_diffuseTexture = 0;
        }

        if (m_specularTexture != 0) {
            glDeleteTextures(1, &m_specularTexture);
            m_specularTexture = 0;
        }

        bool selected = WindowSystem::RenderTexturesDialog(m_materialSelection, m_texturePaths);

        // Kontrola, zda všechny cesty obsahují podporovanou příponu textur
        if (selected) {
            if (m_materialSelection.diffuse.has_value()) {
                if (m_materialSelection.diffuse.value() != "") {
                    if (m_diffuseTexture == 0) {
                        m_diffuseTexture = loadTexture(m_materialSelection.diffuse.value().c_str(),
                                                       GL_LINEAR,
                                                       WindowSystem::s_flipTexture);
                        m_model->SetDiffuseTex(m_diffuseTexture);
                    }
                    else {
                        updateTexture(m_diffuseTexture,
                                      m_materialSelection.diffuse.value().c_str(),
                                      WindowSystem::s_flipTexture);
                    }

                    m_waitingForUserInput = USER_INPUT::NO_INPUT;
                }
                else {
                    m_waitingForUserInput = USER_INPUT::COLOR_SELECTION;
                }
            }

            if (m_materialSelection.specular.has_value()) {
                if (m_materialSelection.specular.value() != "") {
                    Log::Info("Specular texture: " + m_materialSelection.specular.value());
                    if (m_specularTexture == 0) {
                        m_specularTexture
                            = loadTexture(m_materialSelection.specular.value().c_str(),
                                          GL_LINEAR,
                                          WindowSystem::s_flipTexture);
                        m_model->SetSpecularTex(m_specularTexture);
                    }
                    else {
                        updateTexture(m_specularTexture,
                                      m_materialSelection.specular.value().c_str(),
                                      WindowSystem::s_flipTexture);
                    }
                }
            }
        }
    }

    if (m_waitingForUserInput == USER_INPUT::COLOR_SELECTION) {
        std::optional<glm::vec3> selectedColor = WindowSystem::RenderModelColorPicker();
        if (selectedColor.has_value()) {
            m_model->OverwriteColor(selectedColor.value());
            m_waitingForUserInput = USER_INPUT::NO_INPUT;
        }
    }

    if (m_model != nullptr && m_waitingForUserInput == USER_INPUT::NO_INPUT) {
        m_materialSelection = MaterialSelection();
        Renderer::GetInstance().RenderModel(*m_model, inputData);
    }
}

bool ModelLoader::IsModelLoaded() {
    return m_model != nullptr;
}

ModelLoader::~ModelLoader() {
    if (m_diffuseTexture != 0) {
        glDeleteTextures(1, &m_diffuseTexture);
        m_diffuseTexture = 0;
    }
    if (m_specularTexture != 0) {
        glDeleteTextures(1, &m_specularTexture);
        m_specularTexture = 0;
    }
}

const std::vector<std::string> ModelLoader::FindTexture(const std::string& modelPath) const {
    std::filesystem::path path(modelPath);

    // Odstranění poslední části cesty
    path = path.parent_path();

    std::vector<std::string> textures;
    std::filesystem::recursive_directory_iterator dir(path), end;

    while (dir != end) {
        if (dir->is_regular_file()) {
            std::string extension = dir->path().extension().string();
            if (std::find(SUPPORTED_TEXTURE_EXTENSIONS.begin(),
                          SUPPORTED_TEXTURE_EXTENSIONS.end(),
                          extension)
                != SUPPORTED_TEXTURE_EXTENSIONS.end()) {
                textures.push_back(dir->path().string());
            }
        }

        // Kontrola, zda 'dir' dosáhl 'end' před inkrementací
        if (++dir == end) {
            break;
        }
    }

    return textures;
}
