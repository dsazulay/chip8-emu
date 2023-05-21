#include <array>
#include <chrono>
#include <thread>
#include <span>

#include "window.h"
#include "renderer.h"
#include "chip8.h"

constexpr const char* FILE_NAME = "../data/space_invaders.ch8";
std::array<uint8_t, SCREEN_WIDTH * SCREEN_HEIGHT * 4> screen;

auto updateScreen(std::span<uint8_t> chip8screen) -> void
{
    for (int i = 0; i < SCREEN_HEIGHT; i++)
    {
        for (int j = 0; j < SCREEN_WIDTH; j++)
        {
            int index = (i * 64 + j) * 4;
            screen[index] = screen[index + 1] = screen[index + 2] = chip8screen[(31 - i) * SCREEN_WIDTH + j] * 255;
            screen[index + 3] = 0xff; 
        }
    }
}

auto main() -> int
{
    Window window;
    window.createWindow(640, 320, "Chip 8 Emulator");
    
    Chip8 chip8;
    chip8.cpuReset();
    chip8.loadROM(FILE_NAME);

    Renderer renderer(SCREEN_WIDTH, SCREEN_HEIGHT);
    
    while (!window.shouldClose())
    {
        auto target_fps = std::chrono::steady_clock::now() + std::chrono::milliseconds(14);
        if (!window.isFocused())
        {
            window.pollEvents();
            // Limit fps when out of focus
            target_fps += std::chrono::milliseconds(140);
            std::this_thread::sleep_until(target_fps);
            continue;
        }

        if (Window::keyPressed != -1)
        {
            chip8.keyPressed(Window::keyPressed);
            Window::keyPressed = -1;
        }
        else if (Window::keyReleased != -1)
        {
            chip8.keyReleased(Window::keyReleased);
            Window::keyReleased = -1;
        }

        chip8.tick();

        if (chip8.shouldItDraw())
            updateScreen(chip8.screenBuffer());
        renderer.render(screen.data());

        window.swapBuffers();
        window.pollEvents();

        std::this_thread::sleep_until(target_fps);
    }
    
    return 0;
}
