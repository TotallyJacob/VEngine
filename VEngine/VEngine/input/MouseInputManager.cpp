#include "MouseInputManager.h"

using namespace vengine;

double MouseInputManager::m_previousXPos = 0;
double MouseInputManager::m_previousYPos = 0;
double MouseInputManager::m_xOffset = 0;
double MouseInputManager::m_yOffset = 0;
double MouseInputManager::m_centerX = 0;
double MouseInputManager::m_centerY = 0;
bool   MouseInputManager::m_centerMouse = false;
bool   MouseInputManager::m_centerMouseJustChanged = false;
bool   MouseInputManager::m_mouseMoved = false;

MouseInputManager::MouseInputManager(GLFWwindow* _window, MouseInputOptions _mouseInputOptions)
{

    m_window = _window;
    m_mouseInputOptions = _mouseInputOptions;

    init();
}

void MouseInputManager::init()
{

    // setting callbacks
    glfwSetCursorPosCallback(m_window, cursorPositionCallback);
    glfwSetCursorEnterCallback(m_window, cursorEnterCallback);
    glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
    glfwSetScrollCallback(m_window, mouseScrollCallback);

    // mouse Options
    MouseInputManager::setMouseInputOptions(m_mouseInputOptions);
}

void MouseInputManager::updateMouseInputOptions()
{
    // Raw mouse Movement
    if (glfwRawMouseMotionSupported())
    {
        glfwSetInputMode(m_window, GLFW_RAW_MOUSE_MOTION, m_mouseInputOptions.raw);
    }

    // Mouse Visibility
    switch (m_mouseInputOptions.visible)
    {
        case true:
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            break;

        case false:
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
            break;
    }
}

// callbacks
void MouseInputManager::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    /*TODO fix this slow code*/

    m_mouseMoved = true;

    m_xOffset = xpos - m_previousXPos;
    m_yOffset = m_previousYPos - ypos;

    if (m_centerMouseJustChanged)
    {

        m_xOffset = 0;
        m_yOffset = 0;

        m_centerMouseJustChanged = false;
    }

    if (m_centerMouse)
    {
        glfwSetCursorPos(window, m_centerX, m_centerY);

        m_previousXPos = m_centerX;
        m_previousYPos = m_centerY;

        return;
    }

    m_previousXPos = xpos;
    m_previousYPos = ypos;
}
void MouseInputManager::cursorEnterCallback(GLFWwindow* window, int entered)
{
}
void MouseInputManager::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
}
void MouseInputManager::mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
}
