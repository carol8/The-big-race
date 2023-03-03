#ifndef Camera_hpp
#define Camera_hpp

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

#include <string>

namespace gps {

    enum MOVE_DIRECTION { MOVE_UP, MOVE_DOWN, MOVE_RIGHT, MOVE_LEFT, MOVE_FORWARD, MOVE_BACKWARD };

    class Camera
    {
        float yaw = 0.0f, pitch = 0.0f;
    public:
        //Camera constructor
        Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp);
        Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp, bool anglesChanged);
        glm::vec3 getPosition();
        glm::vec3 getTarget();
        glm::vec3 getUp();
        //return the view matrix, using the glm::lookAt() function
        glm::mat4 getViewMatrix();
        //update the camera internal parameters following a camera move event
        void move(MOVE_DIRECTION direction, float speed);
        //update the camera internal parameters following a camera rotate event
        //yaw - camera rotation around the y axis
        //pitch - camera rotation around the x axis
        void rotate(float pitch, float yaw);

    private:
        glm::vec3 cameraPosition;
        glm::vec3 cameraTarget;
        glm::vec3 cameraFrontDirection;
        glm::vec3 cameraRightDirection;
        glm::vec3 cameraUpDirection;
        bool anglesChanged;
    };

}

#endif /* Camera_hpp */
