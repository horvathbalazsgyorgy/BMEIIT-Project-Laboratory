#include "cinematic.h"

void CinematicCamera::initDump() {
    this->linkUniform("cinematic", &cinematic);
    this->linkUniform("fade", &fade);
}

void CinematicCamera::handleState() {
    if (fade <= 0.0f) {
        timer = 0.0f;
        cinematicState = CinematicState::FADE_IN;
        sequenceCounter = (sequenceCounter + 1) % sequences.size();
        position = sequences[sequenceCounter].startingPosition;
        pitch = sequences[sequenceCounter].rotation.x;
        yaw = sequences[sequenceCounter].rotation.y;
    }

    if (fade >= 1.0f) {
        cinematicState = CinematicState::MOVING;
    }

    if (timer >= duration) {
        cinematicState = CinematicState::FADE_OUT;
    }
}

void CinematicCamera::animate(const float dt) {
    handleState();
    switch (cinematicState) {
        case CinematicState::FADE_IN:
            fade = glm::min(1.0f, fade + dt * 0.75f);
            break;
        case CinematicState::MOVING:
            timer += dt;
            break;
        case CinematicState::FADE_OUT:
            fade = glm::max(0.0f, fade - dt * 0.75f);
            break;
    }
    position += sequences[sequenceCounter].forward * dt * animationSpeed;
    this->update();
}

CinematicCamera::CinematicCamera(
    const std::vector<ShaderProgram*>& programs,
    const glm::vec3 position,
    const float pitch,
    const float yaw,
    const float duration,
    const float animSpeed)
: Camera(programs, position, pitch, yaw, "camera")
{
    timer = 0.0f;
    sequenceCounter = 0;
    cinematic = false;

    fade = 1.0f;
    this->duration = duration;
    this->animationSpeed = animSpeed;

    CinematicCamera::initDump();
}

void CinematicCamera::changeState() {
    cinematic = !cinematic;
    if (cinematic) {
        fade = 0.0f;
        sequenceCounter = sequences.size() - 1;
        savedState.position = position;
        savedState.pitch = pitch;
        savedState.yaw = yaw;
    }else {
        fade = 1.0f;
        position = savedState.position;
        pitch = savedState.pitch;
        yaw = savedState.yaw;
    }
}

void CinematicCamera::move(float dt, const std::set<unsigned int> &keysPressed) {
    if (cinematic) {
        animate(dt);
    }else {
        Camera::move(dt, keysPressed);
    }
}