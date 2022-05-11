#pragma once

// Matrix Util
#include "GLM/glm.hpp"
#include "GLM/gtc/matrix_transform.hpp"
#include "GLM/gtx/transform.hpp"

namespace vengine
{

class Camera
{
    protected:

        // Look At
        glm::mat4       view = glm::mat4(1.0f);
        const glm::vec3 cameraUp = glm::normalize(glm::vec3(0.f, 1.0f, 0.f));
        glm::vec3       cameraTarget = glm::normalize(glm::vec3(0.f, 0.f, -1.0f));
        glm::vec3       cameraPos = glm::vec3(0.f);
        glm::vec3       cameraDir = glm::vec3(0.f, 0.f, 0.f);

    public:

        Camera() = default;
        ~Camera() = default;


        // Methods
        inline void updateLookAt()
        {
            view = glm::lookAt(cameraPos, cameraPos + cameraTarget, cameraUp);
        }

        inline void updateCameraDir()
        {
            cameraDir = cameraPos - cameraTarget;
        }

        // Setters
        inline void setCameraDir(glm::vec3& camDir)
        {
            cameraDir = camDir;
        }

        inline void setPosition(glm::vec3 pos)
        {
            cameraPos = pos;
        }
        inline void setTarget(glm::vec3 target)
        {
            cameraTarget = target;
        }

        // Getters
        inline glm::vec3& getPosition()
        {
            return cameraPos;
        }

        inline glm::vec3& getTarget()
        {
            return cameraTarget;
        }

        inline const glm::vec3& getCameraUp() const
        {
            return cameraUp;
        }

        inline glm::mat4& updateGet()
        {
            updateLookAt();
            return view;
        }

        inline const glm::vec3& getCameraDir() const
        {
            return cameraDir;
        }

        inline const glm::vec3& updateGetCameraDir()
        {
            updateCameraDir();
            return cameraDir;
        }

        // Operator
        operator glm::mat4&()
        {
            return view;
        }
};
} // namespace vengine
