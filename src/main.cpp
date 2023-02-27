#include <cstring>
#include <cstdint>
#include <array>
#include <vector>
#include <fstream>
#include <random>

#include <fmt/core.h>

constexpr int MEM_SIZE = 0x1000;
constexpr int MAX_REGISTERS = 16;
constexpr int SCREEN_WIDTH = 64;
constexpr int SCREEN_HEIGHT = 32;

std::array<uint8_t, MEM_SIZE> m_memory;
std::array<uint8_t, MAX_REGISTERS> m_registers;
uint16_t m_addressI;
uint16_t m_programCounter;
std::vector<uint16_t> m_stack;
std::array<std::array<uint8_t, SCREEN_WIDTH>, SCREEN_HEIGHT> m_screen;
uint16_t m_stackPointer;
std::array<uint8_t, 16> m_key;
uint8_t m_delayTimer;
uint8_t m_soundTimer;

std::random_device m_r;
std::default_random_engine m_randomEngine(m_r());
std::uniform_int_distribution<uint8_t> m_uniformDist(0, 255);

constexpr const char* FILE_NAME = "../data/space_invaders.ch8";

auto cpuReset() -> void
{
    m_addressI = 0;
    m_programCounter = 0x200;
    std::memset(m_registers.data(), 0, sizeof(m_registers));

    std::ifstream in(FILE_NAME, std::ios::binary);
    if (!in.is_open())
    {
        fmt::print("Could not open the file {} for reading", FILE_NAME);
        return;
    }
    in.seekg(0, std::ios::end);
    std::streampos fileSize = in.tellg();
    in.seekg(0, std::ios::beg);

    in.read(reinterpret_cast<char*>(&m_memory.at(0x200)), fileSize);
    in.close();
}

auto getNextOpcode() -> uint16_t 
{
    uint16_t opcode = 0;
    opcode = m_memory.at(m_programCounter);
    opcode <<= 8;
    opcode |= m_memory.at(m_programCounter + 1);
    m_programCounter += 2;
    return opcode;
}

auto drawSprite(uint8_t x, uint8_t y, uint8_t n) -> void
{
    m_registers.at(0xF) = 0;
    for (uint8_t yLine = 0; yLine < n; yLine++)
    {
        uint8_t byte = m_memory.at(m_addressI + yLine);
        for (uint8_t xPixel = 0; xPixel < 8; xPixel++)
        {
            uint8_t bit = (byte >> xPixel) & 0x1;
            uint8_t& pixel = m_screen.at((yLine + y) % SCREEN_HEIGHT).at(((7 - xPixel) + x) % SCREEN_WIDTH);

            if (bit == 1 && pixel == 1)
                m_registers.at(0xF) = 1;

            pixel ^= bit;
        }
    }
}

auto waitKeyPress() -> uint8_t
{
    while (true)
    {
        for (uint8_t i = 0; i < 16; i++)
        {
            if (m_key.at(i))
                return i;
        }
    }
}

auto decodeOpcode(uint16_t opcode) -> void
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
                case 0x0000: // clear screen opcode
                    std::memset(m_screen.data(), 0, sizeof(m_screen));
                    break;
                case 0x000E: // return subroutine opcode
                    m_programCounter = m_stack.back();
                    m_stack.pop_back();
                    break;
                default:
                    break;
            }
            break;
        case 0x1000: // jump to address NNN
            m_programCounter = opcode & 0x0FFF;
            break;
        case 0x2000:
            m_stack.push_back(m_programCounter);
            m_programCounter = opcode & 0x0FFF;
            break;
        case 0x3000:
            if (m_registers.at(x) == (opcode & nn))
                m_programCounter += 2;
            break;
        case 0x4000:
            if (m_registers.at(x) != (opcode & nn))
                m_programCounter += 2;
            break;
        case 0x5000: // skip next if reg[x] == reg[y]
            if (m_registers.at(x) == m_registers.at(y))
                m_programCounter += 2;
            break;
        case 0x6000:
            m_registers.at(x) = nn;
            break;
        case 0x7000:
            m_registers.at(x) += nn;
            break;
        case 0x8000:
            switch (n)
            {
                case 0x0:
                    m_registers.at(x) = m_registers.at(y);
                    break;
                case 0x1:
                    m_registers.at(x) |= m_registers.at(y);
                    break;
                case 0x2:
                    m_registers.at(x) &= m_registers.at(y);
                    break;
                case 0x3:
                    m_registers.at(x) ^= m_registers.at(y);
                    break;
                case 0x4:
                    m_registers.at(0xF) = (static_cast<int>(m_registers.at(x)) + static_cast<int>(m_registers.at(y)) > 255) ? 1 : 0;
                    m_registers.at(x) += m_registers.at(y);
                    break;
                case 0x5:
                    m_registers.at(0xF) = (m_registers.at(x) < m_registers.at(y)) ? 0 : 1;
                    m_registers.at(x) -= m_registers.at(y);
                    break;
                case 0x6:
                    m_registers.at(0xF) = m_registers.at(x) & 0x1;
                    m_registers.at(x) >>= 1;
                    break;
                case 0x7:
                    m_registers.at(0xF) = (m_registers.at(y) < m_registers.at(x)) ? 0 : 1;
                    m_registers.at(x) = m_registers.at(y) - m_registers.at(x);
                    break;
                case 0xE:
                    m_registers.at(0xF) = (m_registers.at(x) >> 7) & 0x1;
                    m_registers.at(x) <<= 1;
                    break;
                default:
                    break;
            }
            break;
        case 0x9000:
            if (m_registers.at(x) != m_registers.at(y))
                m_programCounter += 2;
            break;
        case 0xA000:
            m_addressI = opcode & 0x0FFF;
            break;
        case 0xB000:
            m_programCounter = m_registers.at(0) + (opcode & 0x0FFF);
            break;
        case 0xC000:
            m_registers.at(x) = m_uniformDist(m_randomEngine) & (opcode & 0x00FF);
            break;
        case 0xD000:
            drawSprite(x, y, n);
            break;
        case 0xE000:
            switch(n)
            {
                case 0xE:
                    if (m_key.at(m_registers.at(x)))
                        m_programCounter += 2;
                    break;
                case 0x1:
                    if (!m_key.at(m_registers.at(x)))
                        m_programCounter += 2;
                    break;
                default:
                    break;
            }
            break;
        case 0xF000:
            switch(opcode & 0x00FF)
            {
                case 0x0007:
                    m_registers.at(x) = m_delayTimer;
                    break;
                case 0x000A:
                    m_registers.at(x) = waitKeyPress();
                    break;
                case 0x0015:
                    m_delayTimer = m_registers.at(x);
                    break;
                case 0x0018:
                    m_soundTimer = m_registers.at(x);
                    break;
                case 0x001E:
                    m_addressI += m_registers.at(x);
                    break;
                case 0x0029:
                    m_addressI = m_registers.at(x) * 5;
                    break;
                case 0x0033:
                    m_memory.at(m_addressI) = m_registers.at(x) / 100;
                    m_memory.at(m_addressI + 1) = (m_registers.at(x) % 100) / 10;
                    m_memory.at(m_addressI + 2) = m_registers.at(x) % 10;
                    break;
                case 0x0055:
                    for (uint8_t i = 0; i < x; i++)
                    {
                        m_memory.at(m_addressI + i) = m_registers.at(i);
                    }
                    m_addressI += x + 1;
                    break;
                case 0x0065:
                    for (uint8_t i = 0; i < 16; i++)
                    {
                        m_memory.at(i) = m_memory.at(m_addressI + i);
                    }
                    m_addressI += x + 1;
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

auto tick() -> void
{
    if (m_delayTimer > 0)
        m_delayTimer--;
    if (m_soundTimer > 0)
    {
        m_soundTimer--;
        if (m_soundTimer == 0)
            fmt::print("beep\n");
    }
}

auto main() -> int
{
    cpuReset();

    while (true)
    {
        decodeOpcode(getNextOpcode());
        tick();
    }
    return 0;
}
