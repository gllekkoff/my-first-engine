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

    // Constructors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), 
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), 
           float yaw = YAW, 
           float pitch = PITCH);

    Camera(float posX, float posY, float posZ, 
           float upX, float upY, float upZ, 
           float yaw, float pitch);

    // Public methods
    [[nodiscard]] glm::mat4 GetViewMatrix() const;
    void ProcessKeyboard(DIRECTIONS direction, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void ProcessMouseScroll(float yoffset);

private:
    void updateCameraVectors();
};

#endif //MY_FIRST_ENGINE_CAMERA_H
