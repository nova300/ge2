#include "engine.h"
#include "shaders.h"

#include "term.h"
#include "systems.h"

static Program *this = NULL;
static float countdown = 5;

char reinit;

void selftest_key_input_poll(void);


int selftest_init()
{
    terminal_clear();
    terminal_print("press F1 to load boid program or F2 to load test program\n");
    terminal_print("or press ESCAPE to exit\n");
    fb_unload_bg();
    reinit = false;
    
}

int selftest_update(float deltaTime)
{
    if (reinit)
    {
        selftest_init();
    }

    countdown = countdown + deltaTime;

    int len = snprintf(NULL, 0, "%f", countdown);
    char *result = malloc(len + 1);
    snprintf(result, len + 1, "%f", countdown);
    terminal_display(result);
    free(result);


}

int selftest_destroy()
{
    free(this);
    this = NULL;
}

int selftest_keyCallback(int key, int action)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
    {
        program_push(program_get_boidmode());
        reinit = true;
    }

    if (key == GLFW_KEY_F2 && action == GLFW_PRESS)
    {
        program_push(program_get_testmode());
        reinit = true;
    }
}

int selftest_mouseCallback(double xpos, double ypos)
{

}

int selftest_scrollCallback(double xoffset, double yoffset)
{

}

void selftest_key_input_poll(void)
{

}


Program *program_get_selftest()
{
    if (this != NULL) return this;
    this = malloc(sizeof(Program));

    this->init = selftest_init;
    this->update = selftest_update;
    this->destroy = selftest_destroy;

    this->keyCallback = selftest_keyCallback;
    this->mouseCallback = selftest_mouseCallback;
    this->scrollCallback = selftest_scrollCallback;
    
    return this;
}