#include "KeyboardInputManager.h"

using namespace vengine;

bool KeyboardInputManager::m_keyDown[KEY_RANGE] = {false};
bool KeyboardInputManager::m_keyChangePress[KEY_RANGE] = {false};
bool KeyboardInputManager::m_keyChangeRelease[KEY_RANGE] = {false};

KeyboardInputManager::KeyboardInputManager(GLFWwindow* window)
{

    init(window);
}


// public
void KeyboardInputManager::init(GLFWwindow* window)
{
    // glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwSetKeyCallback(window, callback);
}

void KeyboardInputManager::updateKeyChanges()
{
    for (unsigned int key = 0; key < KEY_RANGE; key++)
    {
        m_keyChangePress[key] = false;
        m_keyChangeRelease[key] = false;
    }
}

// private
void KeyboardInputManager::callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    switch (action)
    {

        case GLFW_PRESS:

            m_keyChangePress[key] = true;
            m_keyDown[key] = true;
            break;

        case GLFW_RELEASE:

            m_keyChangeRelease[key] = true;
            m_keyDown[key] = false;
            break;

        default:
            break;
    }
}
