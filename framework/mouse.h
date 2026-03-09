#ifndef PROJECTLABORATORY_MOUSE_H
#define PROJECTLABORATORY_MOUSE_H

#include "glm/glm.hpp"
#include "glm/gtc/epsilon.hpp"

using namespace glm;

class Mouse {
    bool mouseDown = false;
    vec2 lastMouse = vec2(0.0f);
    vec2 mouseDelta = vec2(0.0f);

    void calculateCursorPos(vec2 cursor) {
        mouseDelta.x = cursor.x - lastMouse.x;
        mouseDelta.y = lastMouse.y - cursor.y;

        lastMouse = cursor;
    }
public:
    void setMouse(bool mouse) {
        mouseDown = mouse;
    }
    void setCursor(vec2 cursor) {
        calculateCursorPos(cursor);
    }

    bool isMouseDown() const {
        return mouseDown;
    }
    vec2 getMouseDelta() {
        vec2 delta = mouseDelta;
        mouseDelta = vec2(0.0f);
        return delta;
    }
};

inline Mouse mouse;

#endif //PROJECTLABORATORY_MOUSE_H