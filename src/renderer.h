#pragma once

#include <vector>

#include <glm/vec2.hpp>

#include "shader.h"
#include "texture.h"
#include "model.h"


class Renderer
{
public:
    Renderer(unsigned char* data);
    auto render(unsigned char* screen) -> void;
    auto reloadShaders() -> void;

private:
    auto initBackgroundMesh() -> void;
    auto createUBO() -> void;
    auto setCameraUBO() -> void;
    auto setShaderUniformBlock() -> void;
    auto setShaderUniforms() -> void;
    auto renderBackground() -> void;
    auto clear() -> void;

    glm::mat4 m_proj;
    Shader* m_shader;
    Shader* m_unlitShader;
    Texture* m_texture;

    Model* m_backgroundModel;

    std::vector<Shader*> m_shaders;

    unsigned int UBO;
    unsigned int VBO, EBO, VAO;
    unsigned int VBO_BG, EBO_BG, VAO_BG;
};
