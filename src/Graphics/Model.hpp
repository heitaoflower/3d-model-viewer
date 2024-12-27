#pragma once
#include "../Tools/Log.hpp"
#include "Mesh.hpp"
#include "TextureLoader.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <memory>

class Model {
  private:
    bool m_manualySetTextures;
    std::string m_directoryPath; // Cesta k modelu
    std::vector<Mesh> m_meshes;  // vektor kompletních Meshů
    std::vector<MeshData::Texture> m_loadedTextures;
    std::vector<std::string> m_loadedTexturesPaths; // Cesty načtených textur
    glm::vec4 m_color;
    void LoadModel(const std::string& directoryPath);     // Načíst model
    void ProcessNode(aiNode* node, const aiScene* scene); // Získání nodů
    Mesh ProcessMesh(aiMesh* mesh,
                     const aiScene* scene); // Získání vertexů z nodů
    std::vector<MeshData::Texture> LoadMaterialTextures(aiMaterial* mat,
                                                        aiTextureType type,
                                                        std::string typeName); // Získání Textur
    glm::mat4 m_model;
    int32_t m_specularTex;
    int32_t m_diffuseTex;

  public:
    Model(const std::string& directoryPath,
          bool manualySetTextures = false); // Konstruktor s cestou k modelu jako argument
    void DrawArrays() const;

    void OverwriteTexture(int32_t texture);
    void OverwriteColor(glm::vec3 color);
    void SetSpecularTex(int32_t specularTex);
    void SetDiffuseTex(int32_t diffuseTex);
    [[nodiscard]] uint32_t GetVerticesCount() const;
    [[nodiscard]] int32_t GetSpecularTex() const;
    [[nodiscard]] int32_t GetDiffuseTex() const;
    [[nodiscard]] uint32_t GetIndicesCount() const;
    [[nodiscard]] const glm::vec4 GetColor() const;
    [[nodiscard]] const std::string& GetDirectoryPath() const;
    ~Model();
};
