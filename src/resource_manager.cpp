#include "resource_manager.h"

#include <fstream>
#include <sstream>
#include <unordered_map>
#include <filesystem>

#include <fmt/core.h>

//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"

//#define TINYOBJLOADER_IMPLEMENTATION
//#include "tiny_obj_loader.h"

std::map<std::string, Shader> ResourceManager::shaders;
std::map<std::string, Texture> ResourceManager::textures;
std::map<std::string, Model> ResourceManager::models;

auto ResourceManager::loadShader(const char *vertShaderFile, const char *fragShaderFile, std::string name) -> Shader*
{
    std::string vertexCode;
    std::string fragCode;

    try
    {
        std::ifstream vertexFile;
        vertexFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        vertexFile.open(vertShaderFile);

        std::ifstream fragFile;
        fragFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fragFile.open(fragShaderFile);

        std::stringstream vertexStream, fragStream;
        vertexStream << vertexFile.rdbuf();
        fragStream << fragFile.rdbuf();

        vertexFile.close();
        fragFile.close();

        vertexCode = vertexStream.str();
        fragCode = fragStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        fmt::print("Shader file not successfully read!\n");
    }

    if (shaders.contains(name))
    {
        shaders[name].compile(vertexCode.c_str(), fragCode.c_str());
        shaders[name].vertLastWriteTime = std::filesystem::last_write_time(vertShaderFile);
        shaders[name].fragLastWriteTime = std::filesystem::last_write_time(fragShaderFile);
    }
    else
    {
        Shader shader{};
        shader.compile(vertexCode.c_str(), fragCode.c_str());
        shader.vertFilePath = vertShaderFile;
        shader.fragFilePath = fragShaderFile;
        shader.vertLastWriteTime = std::filesystem::last_write_time(vertShaderFile);
        shader.fragLastWriteTime = std::filesystem::last_write_time(fragShaderFile);
        shaders[name] = shader;
    }

    return &shaders[name];
}

auto ResourceManager::recompileShaders() -> void
{
    for (const auto& [name, shader] : shaders)
    {
        if (shader.vertLastWriteTime != std::filesystem::last_write_time(shader.vertFilePath) ||
            shader.fragLastWriteTime != std::filesystem::last_write_time(shader.fragFilePath))
        {
            loadShader(shader.vertFilePath.c_str(), shader.fragFilePath.c_str(), name);
            fmt::print("{} shader recompiled\n", name);
        }
    }
}
auto ResourceManager::loadTexture(unsigned char* data, std::string name) -> Texture*
{
    Texture texture{};

    texture.generate(64, 32, data);

    textures[name] = texture;

    return &textures[name];
}
/*
auto ResourceManager::loadModel(const char *modelFile, std::string name) -> Model*
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelFile)) {
        throw std::runtime_error(warn + err);
    }

    std::unordered_map<Vertex, int> uniqueVertices{};
    Model model;

    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            Vertex vertex{};
            vertex.pos = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            vertex.texCoord = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                attrib.texcoords[2 * index.texcoord_index + 1]
            };

            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(model.vertices.size());
                model.vertices.push_back(vertex);
            }

            model.indices.push_back(uniqueVertices[vertex]);
        }
    }

    models[name] = model;

    return &models[name];
}*/

auto ResourceManager::loadModel(NativeModel type, std::string name) -> Model *
{
    Model model;

    model.vertices.reserve(m_vertices.size());
    model.indices.reserve(m_indices.size());

    for (int i = 0; i < static_cast<int>(m_vertices.size()); i += 4)
    {
        Vertex vertex{};
        vertex.pos = {
            m_vertices[i], m_vertices[i + 1], 0.0f
        };

        vertex.texCoord = {
            m_vertices[i + 2], m_vertices[i + 3]
        };

        model.vertices.emplace_back(vertex);
    }

    for (auto& index : m_indices)
    {
        model.indices.emplace_back(index);
    }

    models[name] = model;

    return &models[name];
}
