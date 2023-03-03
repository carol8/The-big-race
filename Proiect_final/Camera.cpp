#include "Camera.hpp"

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;
        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));
    }

    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp, bool anglesChanged) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;
        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));
        this->cameraUpDirection = glm::normalize(glm::cross(cameraRightDirection, cameraFrontDirection));
        this->anglesChanged = anglesChanged;
        //fprintf(stdout, "camera front: %f %f %f\n", cameraFrontDirection.x, cameraFrontDirection.y, cameraFrontDirection.z);
        //fprintf(stdout, "camera right: %f %f %f\n", cameraRightDirection.x, cameraRightDirection.y, cameraRightDirection.z);
        //fprintf(stdout, "camera up: %f %f %f\n", cameraUpDirection.x, cameraUpDirection.y, cameraUpDirection.z);
    }

    glm::vec3 Camera::getPosition() {
        return this->cameraPosition;
    }

    glm::vec3 Camera::getTarget() {
        return this->cameraTarget;
    }

    glm::vec3 Camera::getUp() {
        return this->cameraUpDirection;
    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraTarget, cameraUpDirection);
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        //TODO
        switch (direction) {
        case MOVE_FORWARD:
            cameraPosition += speed * cameraFrontDirection;
            cameraTarget += speed * cameraFrontDirection;
            break;
        case MOVE_BACKWARD:
            cameraPosition -= speed * cameraFrontDirection;
            cameraTarget -= speed * cameraFrontDirection;
            break;
        case MOVE_RIGHT:
            cameraPosition += speed * cameraRightDirection;
            cameraTarget += speed * cameraRightDirection;
            break;
        case MOVE_LEFT:
            cameraPosition -= speed * cameraRightDirection;
            cameraTarget -= speed * cameraRightDirection;
            break;
        case MOVE_UP:
            cameraPosition += speed * cameraUpDirection;
            cameraTarget += speed * cameraUpDirection;
            break;
        case MOVE_DOWN:
            cameraPosition -= speed * cameraUpDirection;
            cameraTarget -= speed * cameraUpDirection;
            break;
        }
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
        if (this->anglesChanged) {
            
            this->anglesChanged = false;
            this->yaw = glm::acos(glm::dot(glm::normalize(glm::vec3(cameraFrontDirection.x, 0.0f, cameraFrontDirection.z)), glm::vec3(1.0f, 0.0f, 0.0f))) - glm::pi<float>() / 2.0f;
            this->pitch = -glm::acos(cameraFrontDirection.y) + glm::pi<float>() / 2.0f;
        }

        this->yaw += yaw;
        this->pitch += pitch;

        //fprintf(stdout, "yaw: %f, pitch: %f\n", this->yaw, this->pitch);

        //printf("CameraPosition: x = %f, y = %f, z = %f\n", this->cameraPosition.x, this->cameraPosition.y, this->cameraPosition.z);
        //printf("CameraTarget initial: x = %f, y = %f, z = %f\n", this->cameraTarget.x, this->cameraTarget.y, this->cameraTarget.z);

        this->cameraTarget.x = this->cameraPosition.x + glm::sin(-this->yaw) * glm::cos(this->pitch);
        this->cameraTarget.z = this->cameraPosition.z - glm::cos(-this->yaw) * glm::cos(this->pitch);
        this->cameraTarget.y = this->cameraPosition.y + glm::sin(this->pitch);

        //printf("CameraTarget final: x = %f, y = %f, z = %f\n", this->cameraTarget.x, this->cameraTarget.y, this->cameraTarget.z);

        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, glm::vec3(0, 1, 0)));
        this->cameraUpDirection = glm::normalize(glm::cross(cameraRightDirection, cameraFrontDirection));
    }
}