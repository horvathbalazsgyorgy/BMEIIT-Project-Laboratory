#ifndef PROJECTLABORATORY_CINEMATIC_H
#define PROJECTLABORATORY_CINEMATIC_H

#include "framework/framework.h"

using namespace Framework;

struct CameraSequence {
    glm::vec3 startingPosition;
    glm::vec3 forward;
    glm::vec2 rotation;
};

struct CameraSavedState {
    glm::vec3 position;
    float pitch, yaw;
};

enum class CinematicState {
    FADE_IN,
    MOVING,
    FADE_OUT
};

class CinematicCamera : public Camera {
    CinematicState cinematicState{};
    CameraSavedState savedState{};
    std::vector<CameraSequence> sequences;

    float fade, duration, timer, animationSpeed;
    int cinematic, sequenceCounter;

    void initDump() override;
    void handleState();
    void animate(float dt);
public:
    CinematicCamera(
        const std::vector<ShaderProgram*>& programs,
        glm::vec3 position,
        float pitch = 0.0f,
        float yaw   = -90.0f,
        float duration = 5.0f,
        float animSpeed = 2.5f
    );

    void addSequence(const glm::vec3& start, const glm::vec3& forward, const glm::vec2& lookAt) {
        sequences.emplace_back(start, forward, lookAt);
    }
    void changeState();
    void move(float dt, const std::set<unsigned int> &keysPressed) override;
};

#endif //PROJECTLABORATORY_CINEMATIC_H