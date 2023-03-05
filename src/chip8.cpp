#include "chip8.h"

#include <cstring>
#include <fstream>
#include <fmt/core.h>

Chip8::Chip8() : randomEngine(randomDevice()), uniformDistribution(0, 255) {}

auto Chip8::cpuReset() -> void
{
    std::memset(memory.data(), 0, sizeof(uint8_t) * memory.size());
    std::memset(stack.data(), 0, sizeof(uint16_t) * stack.size());
    std::memset(V.data(), 0, sizeof(uint8_t) * V.size());
    std::memset(key.data(), 0, sizeof(uint8_t) * key.size());
    std::memset(gfx.data(), 0, sizeof(uint8_t) * SCREEN_WIDTH * SCREEN_HEIGHT);
    I = 0;
    PC = FIRST_MEM_ADDRESS;
    SP = 0;
    delayTimer = 0;
    soundTimer = 0;
    drawFlag = false;

    for (int i = 0; i < FONTSET_SIZE; i++)
    {
        memory.at(i) = fontset.at(i);
    }
}

auto Chip8::loadROM(std::string_view filename) -> void
{
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open())
    {
        fmt::print("Could not open the file {} for reading", filename);
        return;
    }
    in.seekg(0, std::ios::end);
    std::streampos fileSize = in.tellg();
    in.seekg(0, std::ios::beg);

    in.read(reinterpret_cast<char*>(&memory.at(FIRST_MEM_ADDRESS)), fileSize);
    in.close();
}

auto Chip8::tick() -> void
{
    int instructionsPerFrame = 6;
    for (int i = 0; i < instructionsPerFrame; i++)
        decodeOpcode(getNextOpcode());

    if (delayTimer > 0)
        delayTimer--;
    if (soundTimer > 0)
    {
        soundTimer--;
        if (soundTimer == 0)
            fmt::print("beep\n");
    }
}

auto Chip8::getNextOpcode() -> uint16_t 
{
    uint16_t opcode = 0;
    opcode = memory.at(PC);
    opcode <<= 8;
    opcode |= memory.at(PC + 1);
    PC += 2;
    return opcode;
}

auto Chip8::decodeOpcode(uint16_t opcode) -> void
{
    uint8_t x = (opcode >> 8) & 0x000F;
    uint8_t y = (opcode >> 4) & 0x000F;
    uint8_t n = opcode & 0x000F;
    uint8_t nn = opcode & 0x00FF;

    switch (opcode & 0xF000)
    {
        case 0x0000:
            switch (opcode & 0x000F)
            {
                case 0x0000: // clear screen
                    std::memset(gfx.data(), 0, sizeof(uint8_t) * SCREEN_WIDTH * SCREEN_HEIGHT);
                    drawFlag = true;
                    break;
                case 0x000E: // return subroutine
                    PC = stack.at(--SP);
                    break;
                default:
                    fmt::print("unknown opcode {}", opcode);
                    break;
            }
            break;
        case 0x1000: // jump to address NNN
            PC = opcode & 0x0FFF;
            break;
        case 0x2000:
            stack.at(SP++) = PC;
            PC = opcode & 0x0FFF;
            break;
        case 0x3000:
            if (V.at(x) == (opcode & nn))
                PC += 2;
            break;
        case 0x4000:
            if (V.at(x) != (opcode & nn))
                PC += 2;
            break;
        case 0x5000: // skip next if reg[x] == reg[y]
            if (V.at(x) == V.at(y))
                PC += 2;
            break;
        case 0x6000:
            V.at(x) = nn;
            break;
        case 0x7000:
            V.at(x) += nn;
            break;
        case 0x8000:
            switch (n)
            {
                case 0x0:
                    V.at(x) = V.at(y);
                    break;
                case 0x1:
                    V.at(x) |= V.at(y);
                    break;
                case 0x2:
                    V.at(x) &= V.at(y);
                    break;
                case 0x3:
                    V.at(x) ^= V.at(y);
                    break;
                case 0x4:
                    V.at(0xF) = (static_cast<int>(V.at(x)) + static_cast<int>(V.at(y)) > 255) ? 1 : 0;
                    V.at(x) += V.at(y);
                    break;
                case 0x5:
                    V.at(0xF) = (V.at(x) < V.at(y)) ? 0 : 1;
                    V.at(x) -= V.at(y);
                    break;
                case 0x6:
                    V.at(0xF) = V.at(x) & 0x1;
                    V.at(x) >>= 1;
                    break;
                case 0x7:
                    V.at(0xF) = (V.at(y) < V.at(x)) ? 0 : 1;
                    V.at(x) = V.at(y) - V.at(x);
                    break;
                case 0xE:
                    V.at(0xF) = (V.at(x) >> 7) & 0x1;
                    V.at(x) <<= 1;
                    break;
                default:
                    fmt::print("unknown opcode {}", opcode);
                    break;
            }
            break;
        case 0x9000:
            if (V.at(x) != V.at(y))
                PC += 2;
            break;
        case 0xA000:
            I = opcode & 0x0FFF;
            break;
        case 0xB000:
            PC = (opcode & 0x0FFF) + V.at(0);
            break;
        case 0xC000:
            fmt::print("random\n");
            V.at(x) = uniformDistribution(randomEngine) & (opcode & 0x00FF);
            break;
        case 0xD000:
            drawSprite(V.at(x), V.at(y), n);
            break;
        case 0xE000:
            switch(n)
            {
                case 0xE:
                    if (key.at(V.at(x)))
                        PC += 2;
                    break;
                case 0x1:
                    if (!key.at(V.at(x)))
                        PC += 2;
                    break;
                default:
                    fmt::print("unknown opcode {}", opcode);
                    break;
            }
            break;
        case 0xF000:
            switch(opcode & 0x00FF)
            {
                case 0x0007:
                    V.at(x) = delayTimer;
                    break;
                case 0x000A:
                    V.at(x) = waitKeyPress();
                    break;
                case 0x0015:
                    delayTimer = V.at(x);
                    break;
                case 0x0018:
                    soundTimer = V.at(x);
                    break;
                case 0x001E:
                    //V.at(0xF) = (I + V.at(x) > 0xfff) ? 1 : 0;
                    I += V.at(x);
                    break;
                case 0x0029:
                    I = V.at(x) * 5;
                    break;
                case 0x0033:
                    memory.at(I) = V.at(x) / 100;
                    memory.at(I + 1) = (V.at(x) % 100) / 10;
                    memory.at(I + 2) = V.at(x) % 10;
                    break;
                case 0x0055:
                    for (int i = 0; i <= x; i++)
                    {
                        memory.at(I + i) = V.at(i);
                    }
                    I += x + 1;
                    break;
                case 0x0065:
                    for (int i = 0; i <= x; i++)
                    {
                        V.at(i) = memory.at(I + i);
                    }
                    I += x + 1;
                    break;
                default:
                    fmt::print("unknown opcode {}", opcode);
                    break;
            }
            break;
        default:
            fmt::print("unknown opcode {}", opcode);
            break;
    }
}

auto Chip8::drawSprite(uint8_t x, uint8_t y, uint8_t n) -> void
{
    V.at(0xF) = 0;
    for (int yLine = 0; yLine < n; yLine++)
    {
        uint8_t byte = memory.at(I + yLine);
        for (int xPixel = 0; xPixel < 8; xPixel++)
        {
            uint8_t bit = (byte >> xPixel) & 0x1;
            auto pos = ((yLine + y) % SCREEN_HEIGHT) * SCREEN_WIDTH +
                ((7 - xPixel) + x) % SCREEN_WIDTH;
            //uint8_t& pixel = gfx.at((yLine + y) % SCREEN_HEIGHT).at(((7 - xPixel) + x) % SCREEN_WIDTH);

            uint8_t& pixel = gfx.at(pos);
            if (bit == 1 && pixel == 1)
                V.at(0xF) = 1;

            pixel ^= bit;
        }
    }
    drawFlag = true;
}

auto Chip8::debugDraw() -> void
{
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            if (gfx.at(y * SCREEN_WIDTH + x) == 0) fmt::print("0");
            else fmt::print(" ");
        }
        fmt::print("\n");
    }
    fmt::print("\n");
}

auto Chip8::waitKeyPress() -> uint8_t
{
    while (true)
    {
        for (int i = 0; i < 16; i++)
        {
            if (key.at(i))
                return i;
        }
    }
}

auto Chip8::keyPressed(int k) -> void
{
    key[k] = 1;
}

auto Chip8::keyReleased(int k) -> void
{
    key[k] = 0;
}

auto Chip8::shouldItDraw() -> bool
{
    return drawFlag;
}

auto Chip8::screenBuffer() -> std::span<uint8_t>
{
    return gfx;
}
