//
// Created by enzoc on 30/09/2022.
//

#include "Input.hpp"
#include "../Application.hpp"
#include <GLFW/glfw3.h>
#include <cmath>

namespace Lumen {
    auto Input::IsKeyDown(KeyCode keycode) -> bool {
        GLFWwindow *windowHandle = Application::Get().GetWindowHandle();
        int state = glfwGetKey(windowHandle, static_cast<int>(keycode));
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    auto Input::IsMouseButtonDown(MouseButton button) -> bool {
        GLFWwindow *windowHandle = Application::Get().GetWindowHandle();
        int state = glfwGetMouseButton(windowHandle, static_cast<int>(button));
        return state == GLFW_PRESS;
    }

    auto Input::GetMousePosition() -> glm::vec2 {
        GLFWwindow *windowHandle = Application::Get().GetWindowHandle();

        double x = NAN;
        double y = NAN;
        glfwGetCursorPos(windowHandle, &x, &y);
        return { static_cast<float>(x), static_cast<float>(y) };
    }

    void Input::SetCursorMode(CursorMode mode) {
        GLFWwindow *windowHandle = Application::Get().GetWindowHandle();
        glfwSetInputMode(windowHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL + static_cast<int>(mode));
    }
} // Lumen