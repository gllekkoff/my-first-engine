//
// Created by gllekk on 9/26/25.
//

#ifndef MY_FIRST_ENGINE_CAMERA_H
#define MY_FIRST_ENGINE_CAMERA_H

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

enum DIRECTIONS {
    FRONT,
    BACK,
    LEFT,
    RIGHT,
    UP,
    DOWN,
};

constexpr float PITCH = 0.0f;
constexpr float YAW = -90.0f;
constexpr float SPEED = 2.5f;
constexpr float SENSITIVITY = 0.1f;
constexpr float ZOOM = 45.0f;

class Camera {
public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    float pitch;
    float yaw;

    float cameraSpeed;
    float mouseSensitivity;
    float zoom;

    void ProcessMouseMovement(float xoffset, float yoffset, const bool constrainPitch = true) {
        xoffset *= mouseSensitivity;
        yoffset *= mouseSensitivity;

        yaw   += xoffset;
        pitch += yoffset;

        if (constrainPitch)
        {
            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;
        }

        updateCameraVectors();
    }

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) :
    front(glm::vec3(0.0f, 0.0f, -1.0f)), cameraSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
    {
        this->position = position;
        worldUp = up;
        this->yaw = yaw;
        this->pitch = pitch;
        updateCameraVectors();
    }

    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) :
    front(glm::vec3(0.0f, 0.0f, -1.0f)), cameraSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
    {
        this->position = glm::vec3(posX, posY, posZ);
        worldUp = glm::vec3(upX, upY, upZ);
        this->yaw = yaw;
        this->pitch = pitch;
        updateCameraVectors();
    }

    [[nodiscard]] glm::mat4 GetViewMatrix() const
    {
        return glm::lookAt(position, position + front, up);
    }

    void ProcessKeyboard(const DIRECTIONS direction, const float deltaTime)
    {
        const float velocity = cameraSpeed * deltaTime;
        if (direction == FRONT)
            position += front * velocity;
        if (direction == BACK)
            position -= front * velocity;
        if (direction == LEFT)
            position -= right * velocity;
        if (direction == RIGHT)
            position += right * velocity;
        if (direction == UP)
            position += glm::normalize(worldUp) * velocity;
        if (direction == DOWN)
            position -= glm::normalize(worldUp) * velocity;
    }

    void ProcessMouseScroll(const float yoffset)
    {
        zoom -= yoffset;
        if (zoom < 1.0f)
            zoom = 1.0f;
        if (zoom > 45.0f)
            zoom = 45.0f;
    }

private:
    void updateCameraVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        this->front = glm::normalize(front);
        right = glm::normalize(glm::cross(front, worldUp));
        up    = glm::normalize(glm::cross(right, front));
    }
};


#endif //MY_FIRST_ENGINE_CAMERA_H