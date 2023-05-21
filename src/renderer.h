#pragma once

#include "asset.h"

#include <memory>
#include <vector>


class Renderer
{
public:

    Renderer(int width, int height);
    auto render(unsigned char* screen) -> void;

private:
    auto loadAssets() -> void;
    auto setUniformsAndBindVao() -> void;
    auto renderScreen() -> void;
    auto clear() -> void;

    std::unique_ptr<Model> m_model;
    std::unique_ptr<Texture> m_texture;
    std::unique_ptr<Shader> m_shader;

    int m_width, m_height;
};
