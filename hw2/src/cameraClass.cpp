//
// Created by gllekk on 9/26/25.
//

#include "cameraClass.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)), 
      cameraSpeed(SPEED), 
      mouseSensitivity(SENSITIVITY), 
      zoom(ZOOM) {
    Pos = position;
    WorldUp = up;
    this->yaw = yaw;
    this->pitch = pitch;
    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)), 
      cameraSpeed(SPEED), 
      mouseSensitivity(SENSITIVITY), 
      zoom(ZOOM) {
    Pos = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    this->yaw = yaw;
    this->pitch = pitch;
    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(Pos, Pos + Front, Up);
}

void Camera::ProcessKeyboard(const DIRECTIONS direction, const float deltaTime) {
    const float velocity = cameraSpeed * deltaTime;
    if (direction == FRONT)
        Pos += Front * velocity;
    if (direction == BACK)
        Pos -= Front * velocity;
    if (direction == LEFT)
        Pos -= Right * velocity;
    if (direction == RIGHT)
        Pos += Right * velocity;
    if (direction == UP)
        Pos += glm::normalize(WorldUp) * velocity;
    if (direction == DOWN)
        Pos -= glm::normalize(WorldUp) * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, const bool constrainPitch) {
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (constrainPitch) {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::ProcessMouseScroll(const float yoffset) {
    zoom -= yoffset;
    if (zoom < 1.0f)
        zoom = 1.0f;
    if (zoom > 45.0f)
        zoom = 45.0f;
}

void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    Front = glm::normalize(front);
    Right = glm::normalize(glm::cross(front, WorldUp));
    Up = glm::normalize(glm::cross(Right, front));
}
