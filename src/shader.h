#pragma once

#include <string>
#include <filesystem>

#include <glm/mat4x4.hpp>

class Shader
{
public:
    auto compile(const char* vertexSrc, const char* fragSrc) -> void;
    auto use() const -> void;
    auto setUniformBlock(const char* blockName, int blockID) -> void;
    auto setInt(const std::string& name, const int value) const -> void;
    auto setVec2(const std::string& name, const float x, const float y) const -> void;
    auto setVec3(const std::string& name, const glm::vec3& v) const -> void;
    auto setMat4(const std::string& name, const glm::mat4 &mat) const -> void;

    std::string vertFilePath;
    std::string fragFilePath;
    std::filesystem::file_time_type vertLastWriteTime;
    std::filesystem::file_time_type fragLastWriteTime;

private:
    unsigned int ID;

    constexpr static int logBufferSize = 512;
};
