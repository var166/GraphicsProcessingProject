#include "Camera.hpp"

namespace gps {

    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;
        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));

    }

    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraTarget, this->cameraUpDirection);
    }

    void Camera::move(MOVE_DIRECTION direction, float speed) {
        glm::vec3 front = glm::normalize(cameraTarget - cameraPosition);
        glm::vec3 right = glm::normalize(glm::cross(front, cameraUpDirection));
        glm::vec3 up = glm::normalize(glm::cross(front, right));

        switch (direction) {
            case MOVE_FORWARD:
                cameraPosition += front * speed;
                cameraTarget += front * speed;
                break;
            case MOVE_BACKWARD:
                cameraPosition -= front * speed;
                cameraTarget -= front * speed;
                break;
            case MOVE_RIGHT:
                cameraPosition += right * speed;
                cameraTarget += right * speed;
                break;
            case MOVE_LEFT:
                cameraPosition -= right * speed;
                cameraTarget -= right * speed;
                break;
            case MOVE_UP:
                cameraPosition -= up * speed;
                cameraTarget -= up * speed;
                break;
            case MOVE_DOWN:
                cameraPosition += up * speed;
                cameraTarget += up * speed;
                break;
        }

    }

    void Camera::rotate(float pitch, float yaw) {
        glm::vec3 direction = glm::normalize(cameraTarget - cameraPosition);

        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
        direction = glm::vec3(rotation * glm::vec4(direction, 0.0f));

        glm::vec3 right = glm::normalize(glm::cross(direction, cameraUpDirection));
        rotation = glm::rotate(glm::mat4(1.0f), glm::radians(pitch), right);
        direction = glm::vec3(rotation * glm::vec4(direction, 0.0f));

        cameraTarget = cameraPosition + direction;

    }
    void Camera::moveCameraPosition(glm::vec3 position) {
        cameraPosition = position;
    }
    void Camera::setLookAt(glm::vec3 target) {
        cameraTarget = target;
    }
}
