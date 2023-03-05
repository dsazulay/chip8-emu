#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window
{
public:
    Window();
    Window(const Window& w) = delete;
    Window(Window&& w) = delete;
    auto operator=(const Window& w) -> Window& = delete;
    auto operator=(Window&& w) -> Window& = delete;
    ~Window();

    auto createWindow(int width, int height, const char* name) -> void;
    [[nodiscard]] auto shouldClose() const -> bool;
    [[nodiscard]] auto isFocused() const -> bool;
    auto swapBuffers() const -> void;
    auto pollEvents() -> void;
    auto getWindow() -> GLFWwindow*;

    static int keyPressed;
    static int keyReleased;

private:
    static auto frameBufferCallback(GLFWwindow* window, int width, int height) -> void;
    static auto keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) -> void;

    GLFWwindow* m_window{};
};
