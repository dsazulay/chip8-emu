#include "window.h"

//#include "dispatcher.h"
//#include "event.h"
//#include "keycodes.h"

int Window::keyPressed = -1;
int Window::keyReleased = -1;

Window::Window()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
}

Window::~Window()
{
    glfwTerminate();
}

auto Window::createWindow(int width, int height, const char *name) -> void
{
    m_window = glfwCreateWindow(width, height, name, nullptr, nullptr);
    if (m_window == nullptr)
    {
        //LOG_ERROR("Failed to create GLFW window");
        return;
    }
    glfwMakeContextCurrent(m_window);
    glfwSetFramebufferSizeCallback(m_window, frameBufferCallback);
    glfwSetKeyCallback(m_window, keyboardCallback);

    //ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "Failed to initialize GLAD");
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
}

auto Window::shouldClose() const -> bool
{
    return glfwWindowShouldClose(m_window);
}

auto Window::isFocused() const -> bool
{
    return glfwGetWindowAttrib(m_window, GLFW_FOCUSED);
}

auto Window::swapBuffers() const -> void
{
    glfwSwapBuffers(m_window);
}

auto Window::pollEvents() -> void
{
    glfwPollEvents();
}

auto Window::frameBufferCallback(GLFWwindow* window, int width, int height) -> void
{
    glViewport(0, 0, width, height);
}

auto Window::keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) -> void
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
    
    else if (action == GLFW_PRESS)
    {
        switch(key)
        {
            case GLFW_KEY_X: keyPressed = 0;  break;
            case GLFW_KEY_1: keyPressed = 1;  break;
            case GLFW_KEY_2: keyPressed = 2;  break;
            case GLFW_KEY_3: keyPressed = 3;  break;
            case GLFW_KEY_Q: keyPressed = 4;  break;
            case GLFW_KEY_W: keyPressed = 5;  break;
            case GLFW_KEY_E: keyPressed = 6;  break;
            case GLFW_KEY_A: keyPressed = 7;  break;
            case GLFW_KEY_S: keyPressed = 8;  break;
            case GLFW_KEY_D: keyPressed = 9;  break;
            case GLFW_KEY_Z: keyPressed = 10; break;
            case GLFW_KEY_C: keyPressed = 11; break;
            case GLFW_KEY_4: keyPressed = 12; break;
            case GLFW_KEY_R: keyPressed = 13; break;
            case GLFW_KEY_F: keyPressed = 14; break;
            case GLFW_KEY_V: keyPressed = 15; break;
        }
    }
    else if (action == GLFW_RELEASE)
    {
        switch(key)
        {
            case GLFW_KEY_X: keyReleased = 0;  break;
            case GLFW_KEY_1: keyReleased = 1;  break;
            case GLFW_KEY_2: keyReleased = 2;  break;
            case GLFW_KEY_3: keyReleased = 3;  break;
            case GLFW_KEY_Q: keyReleased = 4;  break;
            case GLFW_KEY_W: keyReleased = 5;  break;
            case GLFW_KEY_E: keyReleased = 6;  break;
            case GLFW_KEY_A: keyReleased = 7;  break;
            case GLFW_KEY_S: keyReleased = 8;  break;
            case GLFW_KEY_D: keyReleased = 9;  break;
            case GLFW_KEY_Z: keyReleased = 10; break;
            case GLFW_KEY_C: keyReleased = 11; break;
            case GLFW_KEY_4: keyReleased = 12; break;
            case GLFW_KEY_R: keyReleased = 13; break;
            case GLFW_KEY_F: keyReleased = 14; break;
            case GLFW_KEY_V: keyReleased = 15; break;
        }
    }
}

auto Window::getWindow() -> GLFWwindow*
{
    return m_window;
}
