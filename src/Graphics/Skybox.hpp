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
    void RenderSkybox();
    [[nodiscard]] Shader& GetSkyboxShader() const;
    [[nodiscard]] uint32_t GetVAO() const;
    [[nodiscard]] uint32_t GetCubeMapTex() const; 
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
