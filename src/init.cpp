#include "engine.h"

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n", (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
}

void frameBufferSizeCallback(GLFWwindow* window,int width,int height) 
{
    glViewport(0,0,width,height);
    projectionMatrix = matrix_perspective(radians(fov), (float)width/height, 0.1f, 100.0f);
    s_width = width;
    s_height = height;
}

int init()
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
    {
        return 4;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, TITLE, NULL, NULL);

    if (window == NULL)
    {
        return 2;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if ( glewInit() != GLEW_OK )
    {
        return 4;
    }

    if ( initGL() )
    {
        printf("gl init error\n");
        return 5;
    }
    
    return 0;
}


void quit()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

int initGL()
{
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);

    glfwSwapInterval(1);

    glfwSetKeyCallback(window, key_callback);

    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback); 

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return 0;
}


