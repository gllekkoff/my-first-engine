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
constexpr float SPEED = 10.0f;
constexpr float SENSITIVITY = 0.1f;
constexpr float ZOOM = 45.0f;

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
    Front(glm::vec3(0.0f, 0.0f, -1.0f)), cameraSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
    {
        Pos = position;
        WorldUp = up;
        this->yaw = yaw;
        this->pitch = pitch;
        updateCameraVectors();
    }

    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) :
    Front(glm::vec3(0.0f, 0.0f, -1.0f)), cameraSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
    {
        Pos = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        this->yaw = yaw;
        this->pitch = pitch;
        updateCameraVectors();
    }

    [[nodiscard]] glm::mat4 GetViewMatrix() const
    {
        return glm::lookAt(Pos, Pos + Front, Up);
    }

    void ProcessKeyboard(const DIRECTIONS direction, const float deltaTime)
    {
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
        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(front, WorldUp));
        Up    = glm::normalize(glm::cross(Right, front));
    }
};


#endif //MY_FIRST_ENGINE_CAMERA_H