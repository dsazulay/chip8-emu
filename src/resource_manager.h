#pragma once

#include <map>
#include <array>
#include <string>

#include "shader.h"
#include "texture.h"
#include "model.h"

enum class NativeModel
{
    Quad
};

class ResourceManager
{
public:
    static auto loadShader(const char *vertShaderFile, const char *fragShaderFile, std::string name) -> Shader*;
    static auto recompileShaders() -> void;
    static auto loadTexture(unsigned char* data, std::string name) -> Texture*;
    static auto loadModel(const char* modelFile, std::string name) -> Model*;
    static auto loadModel(NativeModel type, std::string name) -> Model*;


    static std::map<std::string, Shader> shaders;
    static std::map<std::string, Texture> textures;
    static std::map<std::string, Model> models;
private:
    ResourceManager() = default;

    static constexpr std::array<float, 16> m_vertices{
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f
    };

    static constexpr std::array<int, 6> m_indices{
        0, 1, 2, 2, 3, 0
    };
};
