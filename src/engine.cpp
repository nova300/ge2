#define SHADERS_H_IMPLEMENTATION
#include "shaders.h"

#include "engine.h"

#include "term.h"
#include "systems.h"
#include <string.h>

#include "selftest.h"

double appTime = 0;
double deltaTime = 0;

GLFWwindow *window;
int exitLoop = 0;
float degrees = 0;

mat4 projectionMatrix;
mat4 viewMatrix;

float4 c_pos = float4(0, 0, 50, 0);
float4 c_front = float4(0, 0, -1, 0);
float4 c_up = float4(0, 1, 0, 0);

float fov = 60.0f;
int s_width = SCREEN_WIDTH;
int s_height = SCREEN_HEIGHT;

Program **programStack;
int programCapacity;
int programTop;

int main(void)
{
    printf("init: ");
    int err = init();
    if (err != 0)
    {
        err = err + 100;
        printf("error %i\n", err);
    }
    printf("ok\n");

    float4x4 m = matrix_perspective(radians(fov), (float)SCREEN_WIDTH/SCREEN_HEIGHT, 0.1f, 100.0f);
    store(m, (float*)&projectionMatrix);

    float4 eye = float4(5, 5, 5, 0);
    float4 center = float4(0, 0, 0, 0);
    float4 up = float4(0, 1, 0, 0);
    float4x4 vm = matrix_lookAt(eye, center, up);
    store(vm, (float*)&viewMatrix);

    fb_init();
    terminal_init();

    program_init();

    program_push(new SelfTest());

    while (exitLoop == 0)
    {
        float4x4 vm = matrix_lookAt(eye, center, up);
        store(vm, (float*)&viewMatrix);
        deltaTime = glfwGetTime() - appTime;
        if (deltaTime > 10) deltaTime = 10;
        appTime = glfwGetTime();

        glClearColor(0.0f, 0.0f, 0.2f, 0.0f);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        fb_render_bg();

        program_update(deltaTime);

        terminal_render();
        fb_render_hi();
        //fb_render();




        glfwSwapBuffers(window);
        //fb_clear();
        glfwPollEvents();
        if (glfwWindowShouldClose(window)) exitLoop = 1;
    }


    quit();
    printf("Goodbye.\n");
    return 0;
}

void helloworld()
{
    printf("hello world");
    return;
}

int program_init()
{
    programCapacity = 3;
    programStack = (Program**)malloc(programCapacity * sizeof(Program *));
    programTop = -1;
    return 0;
}

int program_push(Program *program)
{
    if (programTop + 1 == programCapacity)
    {
        programCapacity *= 2;
        programStack = (Program**)realloc(programStack, programCapacity * sizeof(Program *));
    }
    programTop++;
    programStack[programTop] = program;
    program->init();
    return programTop;
}

int program_pop()
{
    if (programTop == -1) return 0;
    Program *top = programStack[programTop];
    top->destroy();
    programStack[programTop] = NULL;
    programTop--;
    return programTop;
}

int program_update(float deltatime)
{
    if(programTop == -1) return 1;
    Program *top = programStack[programTop];
    top->update(deltatime);
    return 1;
}

Program *program_get()
{
    if(programTop == -1) return NULL;
    Program *top = programStack[programTop];
    return top;
}