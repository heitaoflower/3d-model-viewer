#pragma once
#include "stb_image.h"
#include "../Tools/Log.hpp"
#include "Shader.hpp"
#include <vector>

class Skybox {
  public:
    Skybox(const std::vector<std::string>& cubemapTextures);
    Skybox(const std::string& folderDirectoryPath, const std::string& fileFormat);
    ~Skybox();
    [[nodiscard]] Shader& GetSkyboxShader() const;

  private:
    std::vector<std::string> m_cubemapTextures;
    uint32_t m_textureID;
    uint32_t m_vbo;
    uint32_t m_vao;
    uint32_t LoadTextureFromFile(std::vector<std::string> cubemapTextures);
    void CreateCube();
    void CreateShader();
    Shader* m_cubemapShader;
};
