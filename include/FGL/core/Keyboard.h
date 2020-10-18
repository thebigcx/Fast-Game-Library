#pragma once

#include <GLFW/glfw3.h>

#include "Application.h"

class Keyboard
{
public:
    static bool isKeyPressed(int key)
    {
        return glfwGetKey(Application::get().getWindow().getNative(), key) == GLFW_PRESS;
    }
};