#ifndef PROJECTLABORATORY_MOUSE_H
#define PROJECTLABORATORY_MOUSE_H

#include "glm/glm.hpp"

using namespace glm;

class Mouse {
    bool mouseDown = false;
    vec2 cursorPos = vec2();

    void calculateCursorPos(vec2 cursor) {
        vec2 mouseDelta;
        mouseDelta.x = cursor.x - cursorPos.x;
        mouseDelta.y = cursorPos.y - cursor.y;
        cursorPos = mouseDelta;
    }
public:
    void setMouse(bool mouse) {
        mouseDown = mouse;
    }
    void setCursor(vec2 cursor) {
        calculateCursorPos(cursor);
    }

    [[nodiscard]] bool isMouseDown() const {
        return mouseDown;
    }
    [[nodiscard]] vec2 getCursorPos() const {
        return cursorPos;
    }
};

inline Mouse mouse;

#endif //PROJECTLABORATORY_MOUSE_H