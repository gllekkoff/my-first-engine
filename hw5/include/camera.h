//
// Created by gllekk on 9/26/25.
//

#ifndef MY_FIRST_ENGINE_CAMERA_H
#define MY_FIRST_ENGINE_CAMERA_H

#include "glm/glm.hpp"

enum DIRECTIONS {
    FRONT,
    BACK,
    LEFT,
    RIGHT,
    UP,
    DOWN,
};

class Camera {
public:
    glm::vec3 Pos;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    float pitch;
    float yaw;
    float cameraSpeed;
    float mouseSensitivity;
    float zoom;

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = -90.0f,
           float pitch = 0.0f);

    Camera(float posX, float posY, float posZ,
           float upX, float upY, float upZ,
           float yaw, float pitch);

    glm::mat4 GetViewMatrix() const;
    void ProcessKeyboard(DIRECTIONS direction, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void ProcessMouseScroll(float yoffset);

private:
    void updateCameraVectors();
};

#endif
