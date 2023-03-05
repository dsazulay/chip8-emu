#pragma once

class Texture
{
public:
    auto bind() const -> void;
    auto generate(int width, int height, unsigned char* data) -> void;
    auto update(unsigned char* data) -> void;

    unsigned int ID;
};
