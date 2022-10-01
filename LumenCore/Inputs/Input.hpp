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
        static bool IsKeyDown(KeyCode keycode);

        static bool IsMouseButtonDown(MouseButton button);

        static glm::vec2 GetMousePosition();

        static void SetCursorMode(CursorMode mode);
    };

} // Lumen

#endif //LUMEN_INPUT_HPP