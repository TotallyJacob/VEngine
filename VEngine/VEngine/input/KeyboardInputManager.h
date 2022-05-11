#pragma once


#include "GLFW/glfw3.h"
#include <iostream>

#define KEY_RANGE (GLFW_KEY_LAST + 1)

namespace vengine
{

class KeyboardInputManager
{

    private:

        static bool m_keyDown[KEY_RANGE];
        static bool m_keyChangePress[KEY_RANGE];
        static bool m_keyChangeRelease[KEY_RANGE];

        static void callback(GLFWwindow* window, int key, int scancode, int action, int mods);

    public:


        // Just calls init()
        KeyboardInputManager(GLFWwindow* window);

        void init(GLFWwindow* window);

        // Use this to correctly be able to get m_keyChange... for multiple differnt getKeyChange calls.
        // Must be called before poll events
        static void updateKeyChanges();

        // Getters
        static bool keyChangePress(unsigned int key)
        {
            return m_keyChangePress[key];
        }
        static bool keyChangeRelease(unsigned int key)
        {
            return m_keyChangeRelease[key];
        }
        static bool keyPressed(unsigned int key)
        {
            return m_keyDown[key];
        }
};
} // namespace vengine
