#include "engine.h"
#include "systems.h"



void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    
    
    Program *p = program_get();
    if (p != NULL)
    {
        p->keyCallback(key, action);
    }

}



void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

    Program *p = program_get();
    if (p != NULL)
    {
        p->mouseCallback(xpos, ypos);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    Program *p = program_get();
    if (p != NULL)
    {
        p->scrollCallback(xoffset, yoffset);
    }
}


void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

