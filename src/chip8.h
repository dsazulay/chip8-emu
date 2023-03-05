#pragma once

#include <array>
#include <cstdint>
#include <string_view>
#include <random>
#include <span>

constexpr const int MEM_SIZE = 4096;
constexpr const int STACK_SIZE = 16;
constexpr const int REGISTER_SIZE = 16;
constexpr const int KEY_SIZE = 16;
constexpr const int SCREEN_WIDTH = 64;
constexpr const int SCREEN_HEIGHT = 32;
constexpr const int FONTSET_SIZE = 80;
constexpr const uint16_t FIRST_MEM_ADDRESS = 0x200;
constexpr const std::array<uint8_t, FONTSET_SIZE> fontset =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F 
};

class Chip8
{
public:
    Chip8();
    auto cpuReset() -> void;
    auto loadROM(std::string_view filename) -> void;
    auto tick() -> void;
    auto keyPressed(int k) -> void;
    auto keyReleased(int k) -> void;
    auto shouldItDraw() -> bool;
    auto screenBuffer() -> std::span<uint8_t>;

private:
    auto getNextOpcode() -> uint16_t;
    auto decodeOpcode(uint16_t opcode) -> void;
    auto drawSprite(uint8_t x, uint8_t y, uint8_t n) -> void;
    auto waitKeyPress() -> uint8_t;
    auto debugDraw() -> void;

    std::array<uint8_t, MEM_SIZE> memory;
    std::array<uint16_t, STACK_SIZE> stack;
    std::array<uint8_t, REGISTER_SIZE> V;
    std::array<uint8_t, KEY_SIZE> key;
    std::array<uint8_t, SCREEN_WIDTH * SCREEN_HEIGHT> gfx;
    uint16_t I;
    uint16_t PC;
    uint8_t SP;
    uint8_t delayTimer;
    uint8_t soundTimer;
    bool drawFlag;

    std::random_device randomDevice;
    std::default_random_engine randomEngine;
    std::uniform_int_distribution<uint8_t> uniformDistribution;
};

