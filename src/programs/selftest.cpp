#include "selftest.h"

#include "boidmode.h"
#include "testmode.h"

void SelfTest::init()
{
    terminal_clear();
    terminal_print("press F1 to load boid program or F2 to load test program\n");
    terminal_print("or press ESCAPE to exit\n");
    fb_unload_bg();
    reinit = false;
    
}

void SelfTest::update(float deltaTime)
{
    if (reinit)
    {
        init();
    }

    countdown = countdown + deltaTime;

    int len = snprintf(NULL, 0, "%f", countdown);
    char *result = (char*)malloc(len + 1);
    snprintf(result, len + 1, "%f", countdown);
    terminal_display(result);
    free(result);

}

void SelfTest::destroy()
{

}

void SelfTest::keyCallback(int key, int action)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
    {
        program_push(new BoidMode());
        reinit = true;
    }

    if (key == GLFW_KEY_F2 && action == GLFW_PRESS)
    {
        program_push(new TestMode());
        reinit = true;
    }
}

void SelfTest::mouseCallback(double xpos, double ypos)
{

}

void SelfTest::scrollCallback(double xoffset, double yoffset)
{

}

void SelfTest::key_input_poll(void)
{

}