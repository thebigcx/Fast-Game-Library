#include <events/EventDispatcher.h>

#include <core/Application.h>
#include <events/Event.h>

#include <GLFW/glfw3.h>

static void windowCloseCallback(GLFWwindow* window)
{
    WindowCloseEvent event;
    Application::get().onEvent(event);
}

static void framebufferSizeCallback(GLFWwindow* window, int32_t width, int32_t height)
{
    WindowResizeEvent event(width, height);
    Application::get().onEvent(event);
}

static void windowMaximizeCallback(GLFWwindow* window, int maximized)
{
    if (maximized)
    {
        WindowMaximizeEvent event;
        Application::get().onEvent(event);
    }
    else
    {
        WindowUnmaximizeEvent event;
        Application::get().onEvent(event);
    }
    
}

static void windowPosCallback(GLFWwindow* window, int xpos, int ypos)
{
    WindowPositionEvent event(xpos, ypos);
    Application::get().onEvent(event);
}

static void windowRefreshCallback(GLFWwindow* window)
{
    WindowRefreshEvent event;
    Application::get().onEvent(event);
}

static void windowFocusCallback(GLFWwindow* window, int focused)
{
    if (focused)
    {
        WindowFocuseEvent event;
        Application::get().onEvent(event);
    }
    else
    {
        WindowUnfocuseEvent event;
        Application::get().onEvent(event);
    }
}

static void windowIconifyCallback(GLFWwindow* window, int iconified)
{
    if (iconified)
    {
        WindowMinimizeEvent event;
        Application::get().onEvent(event);
    }
    else
    {
        WindowUnminimizeEvent event;
        Application::get().onEvent(event);
    }
    
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        KeyPressedEvent event(key, 0, mods);
        Application::get().onEvent(event);
    }
    else if (action == GLFW_RELEASE)
    {
        KeyReleasedEvent event(key);
        Application::get().onEvent(event);
    }
}

static void characterCallback(GLFWwindow* window, unsigned int codepoint)
{

}

static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    MouseMovedEvent event(xpos, ypos, false);
    Application::get().onEvent(event);
}

static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        MousePressedEvent event(button, x, y);
        Application::get().onEvent(event);
    }
    else if (action == GLFW_RELEASE)
    {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        MouseReleasedEvent event(button, x, y);
        Application::get().onEvent(event);
    }
}

static void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    MouseScrollEvent event(xoffset, yoffset);
    Application::get().onEvent(event);
}

static void mouseEnterCallback(GLFWwindow* window, int entered)
{
    if (entered)
    {
        MouseEnterEvent event;
        Application::get().onEvent(event);
    }
    else
    {
        MouseLeaveEvent event;
        Application::get().onEvent(event);
    }
}

void EventManager::setupCallbacks()
{
    auto window = Application::get().getWindow().getNative();

    glfwSetWindowCloseCallback(window, windowCloseCallback);
    glfwSetWindowMaximizeCallback(window, windowMaximizeCallback);
    glfwSetWindowPosCallback(window, windowPosCallback);
    glfwSetWindowRefreshCallback(window, windowRefreshCallback);
    glfwSetWindowFocusCallback(window, windowFocusCallback);
    glfwSetWindowIconifyCallback(window, windowIconifyCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCharCallback(window, characterCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetScrollCallback(window, mouseScrollCallback);
    glfwSetCursorEnterCallback(window, mouseEnterCallback);
}