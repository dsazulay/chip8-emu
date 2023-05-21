#pragma once

#include <vector>
#include <string_view>

class Model
{
public:
    struct Vector2f
    {
        union
        {
            float x;
            float r;
        };
        union
        {
            float y;
            float g;
        };
    };

    struct Vertex
    {
        Vector2f pos;
        Vector2f texCoord;
    };

    Model(std::vector<Vertex>& vertices, std::vector<int>& indices);
    auto vao() -> unsigned int;
    auto indicesSize() -> int;

private:
    auto initModel(std::vector<Vertex>& vertices, std::vector<int>& indices) -> void;

    unsigned int m_vbo, m_ebo, m_vao;
    unsigned int m_indicesSize;
};

class Texture
{
public:
    Texture(int width, int height);
    auto bind() const -> void;
    auto generate(int width, int height) -> void;
    auto update(unsigned char* data) -> void;

private:
    unsigned int m_id;
    int m_width, m_height;
};

class Shader
{
public:
    Shader(std::string_view vertFile, std::string_view fragFile);
    auto compile(std::string_view vertexSrc, std::string_view fragSrc) -> void;
    auto use() const -> void;
    auto setInt(std::string_view name, int value) const -> void;
    auto setVec3(std::string_view name, float x, float y, float z) const -> void;

private:
    unsigned int m_id;

    constexpr static int logBufferSize = 512;
};
