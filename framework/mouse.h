#ifndef PROJECTLABORATORY_MOUSE_H
#define PROJECTLABORATORY_MOUSE_H

#include "glm/vec2.hpp"

namespace Framework {
    class Mouse {
        bool mouseDown = false;
        glm::vec2 lastMouse = glm::vec2(0.0f);
        glm::vec2 mouseDelta = glm::vec2(0.0f);

        void calculateCursorPos(glm::vec2 cursor) {
            mouseDelta.x = cursor.x - lastMouse.x;
            mouseDelta.y = lastMouse.y - cursor.y;

            lastMouse = cursor;
        }
    public:
        void setMouse(bool mouse) {
            mouseDown = mouse;
        }
        void setCursor(glm::vec2 cursor) {
            calculateCursorPos(cursor);
        }

        bool isMouseDown() const {
            return mouseDown;
        }
        glm::vec2 getMouseDelta() {
            glm::vec2 delta = mouseDelta;
            mouseDelta = glm::vec2(0.0f);
            return delta;
        }
    };

    inline Mouse mouse;
}

#endif //PROJECTLABORATORY_MOUSE_H