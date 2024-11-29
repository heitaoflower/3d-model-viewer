#include "Skybox.hpp"

Skybox::Skybox(const std::vector<std::string>& cubemapTextures) {
    m_cubemapTextures = cubemapTextures;
    m_textureID = Skybox::LoadTextureFromFile(cubemapTextures);
    Skybox::CreateCube();
    Skybox::CreateShader();
}

Skybox::Skybox(const std::string& folderDirectoryPath, const std::string& fileFormat) {
    std::vector<std::string> cubemapsTextures {
        folderDirectoryPath + "/right" + fileFormat, folderDirectoryPath + "/left" + fileFormat,
        folderDirectoryPath + "/top" + fileFormat,   folderDirectoryPath + "/bottom" + fileFormat,
        folderDirectoryPath + "/front" + fileFormat, folderDirectoryPath + "/back" + fileFormat
    };

    m_cubemapTextures = cubemapsTextures;
    m_textureID = Skybox::LoadTextureFromFile(cubemapsTextures);
    Skybox::CreateCube();
    Skybox::CreateShader();
}

Skybox::~Skybox() {
    delete m_cubemapShader;
}

uint32_t Skybox::LoadTextureFromFile(std::vector<std::string> cubemapTextures) {
    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);

    int width, height, colorChannels;
    unsigned char* data = nullptr;
    stbi_set_flip_vertically_on_load(false);

    for (unsigned int i = 0; i < cubemapTextures.size(); i++) {

        data = stbi_load(cubemapTextures[i].c_str(), &width, &height, &colorChannels, 0);
        if (data) {
            GLenum format = 3;
            if (colorChannels == 1)
                format = GL_RED;
            else if (colorChannels == 3)
                format = GL_RGB;
            else if (colorChannels == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0,
                         format,
                         width,
                         height,
                         0,
                         format,
                         GL_UNSIGNED_BYTE,
                         data);

            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // x osa
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // y osa
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); // z osa

            dge::Log::LogInfo("Cubemap Textura: " + cubemapTextures[i] + " uspesne nactena!");
            stbi_image_free(data);
        }
        else {
            dge::Log::LogError("Cubemap Textura : " + cubemapTextures[i] + " nebyla nactena!");
            stbi_image_free(data);
        }
    }
    return m_textureID;
}

void Skybox::CreateCube() {
    float vertices[108] = { // positions
                            -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
                            1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,

                            -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f,
                            -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,

                            1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,
                            1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,

                            -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
                            1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,

                            -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
                            1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,

                            -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f,
                            1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f
    };

    m_vao.BindVAO();
    m_vbo.SetVertices(*vertices, 108);
    m_vao.AddBuffer(3, 0, 3, 0);
}

void Skybox::CreateShader() {
    m_cubemapShader = new Shader("res/engine/shaders/Skybox/cubemap.frag",
                                 "res/engine/shaders/Skybox/cubemap.vert");
}

Shader& Skybox::GetSkyboxShader() const {
    return *m_cubemapShader;
}
