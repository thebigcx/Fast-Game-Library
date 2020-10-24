#pragma once

#include <core/KeyCodes.h>
#include <core/Application.h>
#include <util/math/vector/Vector2.h>

#include <GLFW/glfw3.h>

class Input
{
public:
    enum MouseButton
    {
        Left = GLFW_MOUSE_BUTTON_LEFT,
        Right = GLFW_MOUSE_BUTTON_RIGHT,
        Middle = GLFW_MOUSE_BUTTON_MIDDLE
    };

    static bool isKeyPressed(Key key);

    static Vector2i getMousePosition();
    
    static bool isMousePressed(MouseButton button);
};