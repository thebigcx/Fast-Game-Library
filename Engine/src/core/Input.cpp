#include <core/Input.h>
#include <core/Game.h>
#include <util/io/Files.h>

namespace Engine
{

math::ivec2 Input::getMousePosition()
{
    auto& window = Game::getInstance().getWindow();
    double x, y;
    glfwGetCursorPos(window.getNative(), &x, &y);
    return math::ivec2(x, window.getSize().y - y);
}
    
bool Input::isMousePressed(MouseButton button)
{
    auto& window = Game::getInstance().getWindow();
    return glfwGetMouseButton(window.getNative(), static_cast<uint32_t>(button));
}

bool Input::isKeyPressed(Key key)
{
    return glfwGetKey(Game::getInstance().getWindow().getNative(), static_cast<uint32_t>(key));
}

bool Input::isGamepadButtonPressed(Gamepad gamepad, GamepadButton button)
{
    int count;
    const unsigned char* btns = glfwGetJoystickButtons(static_cast<uint32_t>(gamepad), &count);
    return btns[static_cast<uint32_t>(button)] == GLFW_PRESS;
}

float Input::getGamepadAxis(Gamepad gamepad, GamepadAxis axis)
{
    int count;
    const float* axes = glfwGetJoystickAxes(static_cast<uint32_t>(gamepad), &count);
    return axes[static_cast<uint32_t>(axis)];
}

void Input::setGamepadMappingsFromFile(const std::string& path)
{
    const char* mappings = Files::readFile(path).c_str();
    glfwUpdateGamepadMappings(mappings);
}

}