#include "renderer.h"

#include <glad/glad.h>
#include "window.h"

Renderer::Renderer(int width, int height) : m_width(width), m_height(height)
{
    loadAssets();
    setUniformsAndBindVao();
}

auto Renderer::loadAssets() -> void
{
    std::vector<Model::Vertex> v{
        {{{-1.0f}, {-1.0f}}, {{0.0f}, {0.0f}}},
        {{{ 1.0f}, {-1.0f}}, {{1.0f}, {0.0f}}},
        {{{ 1.0f}, { 1.0f}}, {{1.0f}, {1.0f}}},
        {{{-1.0f}, { 1.0f}}, {{0.0f}, {1.0f}}},
    };
    std::vector<int> i{0, 1, 2, 2, 3, 0};

    m_model = std::make_unique<Model>(v, i);
    m_texture = std::make_unique<Texture>(m_width, m_height);
    m_shader = std::make_unique<Shader>("../resources/unlit.vert", "../resources/unlit.frag");
}

auto Renderer::setUniformsAndBindVao() -> void
{
    glActiveTexture(GL_TEXTURE0);
    m_texture->bind();

    m_shader->use();
    m_shader->setInt("u_main_tex", 0);
    m_shader->setVec3("u_tint", 0.22f, 0.49f, 0.3f);

    glBindVertexArray(m_model->vao());
}

auto Renderer::render(unsigned char* screen) -> void
{
    m_texture->update(screen);
    clear();
    renderScreen();
}

auto Renderer::renderScreen() -> void
{
    glDrawElements(GL_TRIANGLES, m_model->indicesSize(), GL_UNSIGNED_INT, nullptr);
}

auto Renderer::clear() -> void
{
    glClearColor(0.22f, 0.49f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}
