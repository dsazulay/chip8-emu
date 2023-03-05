#include "renderer.h"

#include <string>

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include "resource_manager.h"
#include "window.h"

Renderer::Renderer(unsigned char* data)
{
    m_proj = glm::ortho(0.0f, 640.0f, 0.0f, 320.0f, -1.0f, 1.0f);
    m_unlitShader = ResourceManager::loadShader("../resources/unlit.vert", 
                                                "../resources/unlit.frag",
                                                "UnlitShader");
    
    //m_texture = ResourceManager::loadTexture("../../resources/cards.png", "CardTex");
    
    m_texture = ResourceManager::loadTexture(nullptr, "MainTex");

    m_backgroundModel = ResourceManager::loadModel(NativeModel::Quad, "QuadModel");

    m_shaders.push_back(m_unlitShader);

    initBackgroundMesh();

    createUBO();

    setCameraUBO();
    setShaderUniformBlock();
    setShaderUniforms();
}

auto Renderer::render(unsigned char* screen) -> void
{
    m_texture->update(screen);
    setShaderUniforms();
    clear();
    renderBackground();
}

auto Renderer::reloadShaders() -> void
{
    setShaderUniforms();
}

auto Renderer::renderBackground() -> void
{
    m_shader = m_unlitShader;
    m_shader->use();
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(320.0f, 160.0f, 0.0f));
    model = glm::scale(model, glm::vec3(320, 160, 1));

    m_shader->setMat4("u_model", model);
    m_shader->setVec3("u_tint", glm::vec3(0.22f, 0.49f, 0.3f));

    glBindVertexArray(VAO_BG);
    glDrawElements(GL_TRIANGLES, m_backgroundModel->indices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}


auto Renderer::initBackgroundMesh() -> void
{
    glGenVertexArrays(1, &VAO_BG);
    glGenBuffers(1, &VBO_BG);
    glGenBuffers(1, &EBO_BG);

    glBindVertexArray(VAO_BG);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_BG);
    glBufferData(GL_ARRAY_BUFFER, m_backgroundModel->vertices.size() * sizeof(Vertex), &m_backgroundModel->vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_BG);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_backgroundModel->indices.size() * sizeof(int), &m_backgroundModel->indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, texCoord)));

    glBindVertexArray(0);
}

auto Renderer::createUBO() -> void
{
    glGenBuffers(1, &UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(m_proj), nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

auto Renderer::setCameraUBO() -> void
{
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(m_proj), glm::value_ptr(m_proj));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

auto Renderer::setShaderUniformBlock() -> void
{
    for (auto shader : m_shaders)
    {
        shader->setUniformBlock("Matrices", 0);
    }
}

auto Renderer::setShaderUniforms() -> void
{
    glActiveTexture(GL_TEXTURE0);
    m_texture->bind();

    m_unlitShader->use();
    m_unlitShader->setInt("u_main_tex", 0);
}

auto Renderer::clear() -> void
{
    glClearColor(0.22f, 0.49f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}
