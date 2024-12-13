#include "Mesh.hpp"

#include <utility>

Mesh::Mesh(std::vector<MeshData::Vertex> vertices,
           std::vector<unsigned int> indices,
           std::vector<MeshData::Texture> textures) {
    this->vertices = std::move(vertices);
    this->indices = std::move(indices);
    this->textures = std::move(textures);

    SetMesh();
}

void Mesh::SetMesh() {
    // Generace Bufferù
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);             // Bind VAO
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO); // Bind VBO

    glBufferData(GL_ARRAY_BUFFER,
                 vertices.size() * sizeof(MeshData::Vertex),
                 &vertices[0],
                 GL_STATIC_DRAW); // Data do VBO

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO); // Bind EBO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indices.size() * sizeof(unsigned int),
                 &indices[0],
                 GL_STATIC_DRAW); // Data do EBO

    glEnableVertexAttribArray(0); // Aktivovat VAO na pozici 0
    glVertexAttribPointer(0,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(MeshData::Vertex),
                          reinterpret_cast<void*>(0)); // Do VAO přidat Data
    // vertex normals
    glEnableVertexAttribArray(1); // Aktivovat VAO na pozici 1
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(MeshData::Vertex),
        reinterpret_cast<void*>(offsetof(MeshData::Vertex, normal))); // Do VAO pøidat Data
    // vertex texture coords
    glEnableVertexAttribArray(2); // Aktivovat VAO na pozici 2
    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(MeshData::Vertex),
        reinterpret_cast<void*>(offsetof(MeshData::Vertex, texCoords))); // Do VAO přidat Data

    glBindVertexArray(0); // unBind
}

void Mesh::DrawArrays() const {
    glBindVertexArray(m_VAO);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
}

void Mesh::Destroy() {
    for (auto& texture : textures)
        glDeleteTextures(1, &texture.id);

    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
    glDeleteVertexArrays(1, &m_VAO);
}
