#include "Model.hpp"

#include <utility>

Model::Model(const std::string& directoryPath, bool manualySetTextures)
    : m_specularTex(0)
    , m_diffuseTex(0) {
    m_directoryPath = directoryPath;
    m_manualySetTextures = manualySetTextures;
    Model::LoadModel(directoryPath);
    m_color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    m_model = glm::mat4(1.f);
}

void Model::DrawArrays() const {
    for (const auto& m_meshe : m_meshes) // Každý mesh ve vectoru vykreslit
        m_meshe.DrawArrays();
}

void Model::SetSpecularTex(int32_t specularTex) {
    m_specularTex = specularTex;
}

int32_t Model::GetDiffuseTex() const {
    return m_diffuseTex;
}

void Model::SetDiffuseTex(int32_t diffuseTex) {
    m_diffuseTex = diffuseTex;
}

int32_t Model::GetSpecularTex() const {
    return m_specularTex;
}

void Model::LoadModel(const std::string& directoryPath) {

    Assimp::Importer importer;
    auto scene = importer.ReadFile(directoryPath, aiProcess_Triangulate);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE
        || !scene->mRootNode) // Pokud scene == null
    {
        Log::Error(importer.GetErrorString());
        return;
    }
    else
        Log::Info("Model: " + directoryPath + " uspesne nacten!");

    m_directoryPath = m_directoryPath.substr(0, m_directoryPath.find_last_of('/'));

    Model::ProcessNode(scene->mRootNode, scene);
}

const glm::vec4 Model::GetColor() const {
    return m_color;
}

void Model::ProcessNode(aiNode* node, const aiScene* scene) {
    //  zpracování všech meshů ve scéně
    for (uint32_t i = 0; i < node->mNumMeshes; i++) // Dokud i != počtu Meshů
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]]; // získáni meshe ze scény
        m_meshes.push_back(ProcessMesh(mesh, scene)); // přidání procesovaného meshe do vectoru
    }
    // zpracování všech potomků nodů
    for (uint32_t i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene);
    }
}

Mesh Model::ProcessMesh(aiMesh* mesh,
                        const aiScene* scene) // Z meshů získat vertexy
{
    std::vector<MeshData::Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<MeshData::Texture> textures;

    // Projit každý vertex meshe
    for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
        MeshData::Vertex vertex;
        glm::vec3 verticesVector;

        // získání positions
        verticesVector.x = mesh->mVertices[i].x;
        verticesVector.y = mesh->mVertices[i].y;
        verticesVector.z = mesh->mVertices[i].z;
        vertex.position = verticesVector;

        // získání normals
        if (mesh->HasNormals()) {
            verticesVector.x = mesh->mNormals[i].x;
            verticesVector.y = mesh->mNormals[i].y;
            verticesVector.z = mesh->mNormals[i].z;
            vertex.normal = verticesVector;
        }
        // získání texture coordinates
        if (mesh->mTextureCoords[0])  // Je v modelu texture souřadnice?
        {                             // Objekt může mít vicero texture souřadnic
            glm::vec2 texturesVector; // 2d souřadnice
            texturesVector.x = mesh->mTextureCoords[0][i].x;
            texturesVector.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoords = texturesVector;
        }
        else
            vertex.texCoords = glm::vec2(0.0f, 0.0f); // Pokud nejsou souřadnice, tak 0,0
        vertices.push_back(vertex);                   // Uložit získané vertexy do vectoru
    }

    // získání indices
    for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];

        // Uložit získané indices do indices vectoru
        for (uint32_t j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    if (!m_manualySetTextures) {
        // Získat textury a mapy
        if (mesh->mMaterialIndex >= 0) {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            std::vector<MeshData::Texture> diffuseMaps
                = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

            // TODO: Přidat specular mapy? => blinn-phong v shaderu
            std::vector<MeshData::Texture> specularMaps
                = LoadMaterialTextures(material, aiTextureType_SPECULAR, "specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        }
    }

    return { vertices, indices, textures }; // Vrátí celý jeden Mesh
}

void Model::OverwriteTexture(int32_t texture) {
    m_loadedTextures.clear();
    m_loadedTexturesPaths.clear();
    m_loadedTextures.push_back({ texture, "diffuse", "" });
    m_manualySetTextures = true;
    m_diffuseTex = texture;
}

const std::string& Model::GetDirectoryPath() const {
    return m_directoryPath;
}

void Model::OverwriteColor(glm::vec3 color) {
    m_color = glm::vec4(color, 1.0f);
}

uint32_t Model::GetVerticesCount() const {
    uint32_t vertexCount = 0;
    for (const auto& m_meshe : m_meshes) {
        vertexCount += m_meshe.vertices.size();
    }
    return vertexCount;
}

uint32_t Model::GetIndicesCount() const {
    uint32_t indicesCount = 0;
    for (const auto& m_meshe : m_meshes) {
        indicesCount += m_meshe.indices.size();
    }
    return indicesCount;
}

std::vector<MeshData::Texture> Model::LoadMaterialTextures(aiMaterial* mat,
                                                           aiTextureType type,
                                                           std::string typeName) {
    std::vector<MeshData::Texture> texturesToReturn;
    bool skip = false;
    aiString str;
    std::string filename;

    for (uint32_t i = 0; i < mat->GetTextureCount(type); i++) {
        mat->GetTexture(type, i, &str);
        filename = str.C_Str();
        filename = filename.substr(filename.find_last_of('\\') + 1, filename.length());
        filename = m_directoryPath;
        filename += "/textures/";
        filename += filename;

        for (uint32_t j = 0; j < m_loadedTextures.size(); j++) {
            if (m_loadedTexturesPaths[j] == str.C_Str()) {
                texturesToReturn.push_back(
                    m_loadedTextures[j]); // Vrátí texturu co byla první použita
                skip = true;
                break;
            }
        }
    }
    if (!skip && std::string(str.C_Str()).find_last_of('\\') < 1000) {
        MeshData::Texture texture;
        m_loadedTexturesPaths.emplace_back(str.C_Str());

        texture.id = loadTexture(filename, GL_LINEAR, true);
        texture.type = std::move(typeName);

        texturesToReturn.push_back(texture);
        m_loadedTextures.push_back(texture);
    }

    return texturesToReturn;
}

Model::~Model() {
    for (auto& m_meshe : m_meshes) {
        m_meshe.Destroy();
    }
}
