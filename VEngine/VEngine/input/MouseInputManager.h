#pragma once

#include "GLFW/glfw3.h"
#include <iostream>

namespace vengine
{

class MouseInputManager
{
    public:

        struct MouseInputOptions
        {
                bool raw = false;
                bool visible = true;
        };

    private:

        // variables
        inline static GLFWwindow*       m_window = nullptr;
        inline static MouseInputOptions m_mouseInputOptions = {};
        static double                   m_previousXPos, m_previousYPos, m_xOffset, m_yOffset, m_centerX, m_centerY;
        static bool                     m_centerMouse, m_centerMouseJustChanged, m_mouseMoved;

        // callbacks
        static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
        static void cursorEnterCallback(GLFWwindow* window, int entered);
        static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

        // Other
        static void init();

    public:

        MouseInputManager(GLFWwindow* window, MouseInputOptions mouseInputOptions = {});
        ~MouseInputManager() = default;

        static void updateMouseInputOptions();

        // Setters

        inline static void updateWindowSize(int windowWidth, int windowHeight)
        {
            m_centerX = ((double)windowWidth / 2.f);
            m_centerY = ((double)windowHeight / 2.f);
        }

        inline static void setMouseInputOptions(MouseInputOptions& _mouseInputOptions)
        {
            m_mouseInputOptions = _mouseInputOptions;

            updateMouseInputOptions();
        }

        inline static void centerMouse(bool centerMouse)
        {
            m_centerMouse = centerMouse;
            m_centerMouseJustChanged = true;
        }

        inline static void setMouseInputMode(GLenum mode)
        {
            glfwSetInputMode(m_window, GLFW_CURSOR, mode);
        }

        static void getMousePos(double& previousXPos, double& previousYPos)
        {
            previousXPos = m_previousXPos;
            previousYPos = m_previousYPos;
        }

        static void getMousePosOffset(double& xoffset, double& yoffset)
        {
            if (m_mouseMoved)
            {
                xoffset = m_xOffset;
                yoffset = m_yOffset;

                m_mouseMoved = false;

                return;
            }

            xoffset = 0;
            yoffset = 0;
        }

        MouseInputOptions& getMouseInputOptions() const
        {
            return m_mouseInputOptions;
        }
};

} // namespace vengine
