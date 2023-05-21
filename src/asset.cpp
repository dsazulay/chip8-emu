#include "asset.h"

#include <fmt/core.h>
#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>

Model::Model(std::vector<Model::Vertex>& vertices, std::vector<int>& indices) : m_indicesSize(indices.size())
{
    initModel(vertices, indices);
}

auto Model::vao() -> unsigned int
{
    return m_vao;
}

auto Model::indicesSize() -> int
{
    return m_indicesSize;
}

auto Model::initModel(std::vector<Model::Vertex>& vertices, std::vector<int>& indices) -> void
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Model::Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, texCoord)));

    glBindVertexArray(0);
}

Texture::Texture(int width, int height) : m_width(width), m_height(height)
{
    generate(width, height);
}

auto Texture::bind() const -> void
{
    glBindTexture(GL_TEXTURE_2D, m_id);
}

auto Texture::generate(int width, int height) -> void
{
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
}

auto Texture::update(unsigned char* data) -> void
{
    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
}

Shader::Shader(std::string_view vertFile, std::string_view fragFile)
{
    std::string vertexCode;
    std::string fragCode;

    try
    {
        std::ifstream vertexInputStream;
        vertexInputStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        vertexInputStream.open(vertFile);

        std::ifstream fragInputStram;
        fragInputStram.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fragInputStram.open(fragFile);

        std::stringstream vertexStream, fragStream;
        vertexStream << vertexInputStream.rdbuf();
        fragStream << fragInputStram.rdbuf();

        vertexInputStream.close();
        fragInputStram.close();

        vertexCode = vertexStream.str();
        fragCode = fragStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        fmt::print("Shader file not successfully read!\n");
    }

    compile(vertexCode.c_str(), fragCode.c_str());
}

auto Shader::compile(std::string_view vertexSrc, std::string_view fragSrc) -> void
{
    unsigned int vertexID{}, fragID{};
    int success{};
    std::string infoLog{};
    const char* stringSrc{};

    // TODO: check how to get the address of the string view literal string
    stringSrc = vertexSrc.data();
    vertexID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexID, 1, &stringSrc, nullptr);
    glCompileShader(vertexID);

    glGetShaderiv(vertexID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexID, Shader::logBufferSize, nullptr, infoLog.data());
        fmt::print("Vertex shader compilation failed: {}\n", infoLog);
    }

    stringSrc = fragSrc.data();
    fragID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragID, 1, &stringSrc, nullptr);
    glCompileShader(fragID);

    glGetShaderiv(fragID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragID, Shader::logBufferSize, nullptr, infoLog.data());
        fmt::print("Fragment shader compilation failed: {}\n", infoLog);
    }

    m_id = glCreateProgram();
    glAttachShader(m_id, vertexID);
    glAttachShader(m_id, fragID);

    glLinkProgram(m_id);

    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(m_id, Shader::logBufferSize, nullptr, infoLog.data());
        fmt::print("Shader program linking failed: {}\n", infoLog);
    }

    glDeleteShader(vertexID);
    glDeleteShader(fragID);
}

auto Shader::use() const -> void
{
    glUseProgram(m_id);
}

auto Shader::setInt(std::string_view name, int value) const -> void
{
    glUniform1i(glGetUniformLocation(m_id, name.data()), value);
}

auto Shader::setVec3(std::string_view name, float x, float y, float z) const -> void
{
    glUniform3f(glGetUniformLocation(m_id, name.data()), x, y, z);
}

