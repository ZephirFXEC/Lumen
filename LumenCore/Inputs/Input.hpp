//
// Created by enzoc on 30/09/2022.
//

#ifndef LUMEN_INPUT_HPP
#define LUMEN_INPUT_HPP

#include "KeyCodes.hpp"
#include <glm/glm.hpp>

namespace Lumen {

    class Input {
    public:
        static auto IsKeyDown(KeyCode keycode) -> bool;

        static auto IsMouseButtonDown(MouseButton button) -> bool;

        static auto GetMousePosition() -> glm::vec2;

        static void SetCursorMode(CursorMode mode);
    };

} // Lumen

#endif //LUMEN_INPUT_HPP