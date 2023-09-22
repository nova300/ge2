/*#include "engine.h"
#include "shaders.h"

#include "term.h"
#include "systems.h"


static Program *this = NULL;

typedef struct
{
    GeoObject **rq;
    RenderQueue renderQueue1;
    GeoObject_gpu *obj2;
    float sensitivity;
    float speed;
    char firstMouse;
    float lastX;
    float lastY;
    float yaw;
    float pitch;
    char captureMouse;
}testmode_localdata;

static testmode_localdata *localstorage = NULL;

void testprogram_key_input_poll(void);


int testprogram_init()
{
    glfwSetWindowTitle(window, "test");

    terminal_clear();
    terminal_print("test program no.1 loaded\n");

    localstorage = malloc(sizeof(testmode_localdata));

    localstorage->sensitivity = 0.1f;
    localstorage->speed = 2.0f;
    localstorage->firstMouse = true;
    localstorage->lastX = SCREEN_WIDTH / 2;
    localstorage->lastY = SCREEN_HEIGHT / 2;
    localstorage->yaw = -90.0f;
    localstorage->pitch = 0.0f;
    localstorage->captureMouse = 0;

    vec4 eye = {{0, 0, 5, 0}};
    vec4 center = {{0, 0, -1, 0}};

    c_pos = eye;
    c_front = center;

    skybox_load_texture("media/bitfs2.png");

    GeoObject *gobj = geo_new_object();
    geo_obj_loadFromFile("media/cube.obj", gobj);

    rq_init(&localstorage->renderQueue1, 10);

    Shader *s = newShaderObject(vertex_shader_0, fragment_shader_0);
    //gobj->shader = s;
    localstorage->renderQueue1.gpuHandle.shader = s;

    localstorage->rq = localstorage->renderQueue1.objectBuffer;

    localstorage->renderQueue1.gpuHandle.textureAtlas = generateRandomAtlas();

    gobj->baseTexture = 5;
    rq_add_object(&localstorage->renderQueue1, gobj);

    //GeoObject *cube1 = geo_new_object();
    //geo_obj_loadFromFile("media/cube.obj", cube1);

    //rq_add_object(&localstorage->renderQueue1, cube1);

    vertex_c *verices = malloc(sizeof(vertex_c) * 3);

    verices[0] = gfx_make_color_vertex(1, 1, -1, 1, 1, gfx_make_color(255, 0, 0, 255));
    verices[1] = gfx_make_color_vertex(1, 0, -1, 1, 1, gfx_make_color(0, 255, 0, 255));
    verices[2] = gfx_make_color_vertex(0, 0, -1, 1, 1, gfx_make_color(0, 0, 255, 255));

    unsigned int *indices = malloc(sizeof(unsigned int) * 3);
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;

    GeoObject *gobj2 = geo_new_object();

    gobj2->type = GOBJ_TYPE_COLOR;
    gobj2->data = (vertex*)verices;
    gobj2->dataCount = 3;
    gobj2->indicies = indices;
    gobj2->indexCount = 3;
    
    localstorage->obj2 = geo_obj_bindToGpu(*gobj2);

    geo_obj_gpu_updateBuffers(localstorage->obj2);

    localstorage->obj2->gpuHandle.shader = s;
    localstorage->obj2->gpuHandle.textureAtlas = generateRandomAtlas();


}

int testprogram_update(float deltaTime)
{
    if (localstorage == NULL) return 0;
    testprogram_key_input_poll();
    //transform_rotate(0, 0, 1 * deltaTime, &rq[1]->baseTransform);
    //transform_rotate(1 * deltaTime, 0, 0, &rq[2]->baseTransform);

    // particle_update(p1);

    //rq[2]->baseTexture = rand() % 50;
    //rq[2]->instanceDirty = 1;

    rq_update_buffers(&localstorage->renderQueue1);

    render_skybox();

    geo_render(&localstorage->obj2->gpuHandle);

    //geo_render(&localstorage->renderQueue1.gpuHandle);
}

int testprogram_destroy()
{
    freeShaderObject(localstorage->renderQueue1.gpuHandle.shader);
    glDeleteTextures(1, &localstorage->renderQueue1.gpuHandle.textureAtlas);
    rq_free_with_objects(&localstorage->renderQueue1);
    free(localstorage);
    localstorage == NULL;
    free(this);
    this = NULL;
}

int testprogram_keyCallback(int key, int action)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        program_pop();
    }
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
    {
        if (localstorage->captureMouse)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            localstorage->captureMouse = 0;
            return 0;
        }
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        localstorage->captureMouse = 1;
    }
}

int testprogram_mouseCallback(double xpos, double ypos)
{
    if (!localstorage->captureMouse)
    {
        return 0;
    }
    if (localstorage->firstMouse)
    {
        localstorage->lastX = xpos;
        localstorage->lastY = ypos;
        localstorage->firstMouse = false;
    }
  
    float xoffset = xpos - localstorage->lastX;
    float yoffset = localstorage->lastY - ypos; 
    localstorage->lastX = xpos;
    localstorage->lastY = ypos;

    xoffset *= localstorage->sensitivity;
    yoffset *= localstorage->sensitivity;

    localstorage->yaw += xoffset;
    localstorage->pitch += yoffset;

    if(localstorage->pitch > 89.0f) localstorage->pitch = 89.0f;
    if(localstorage->pitch < -89.0f) localstorage->pitch = -89.0f;

    vec4 direction;
    direction.x = cos(radians(localstorage->yaw)) * cos(radians(localstorage->pitch));
    direction.y = sin(radians(localstorage->pitch));
    direction.z = sin(radians(localstorage->yaw)) * cos(radians(localstorage->pitch));
    direction.w = 0;
    vector_normalize(&direction);
    c_front = direction;
}

int testprogram_scrollCallback(double xoffset, double yoffset)
{
    fov = fov - (yoffset * 10);
    if (fov < 1.0f) fov = 1.0f;
    if (fov > 120.0f) fov = 120.0f; 
    projectionMatrix = matrix_perspective(radians(fov), (float)s_width/s_height, 0.1f, 100.0f);
}

void testprogram_key_input_poll(void)
{
    float c_speed = localstorage->speed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        c_speed = c_speed * 2;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        c_pos = vector_add(c_pos, vector_scale(c_front, c_speed));
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        c_pos = vector_subtract(c_pos, vector_scale(c_front, c_speed));
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {   
        vec4 m = vector_cross(c_front, c_up);
        vector_normalize(&m);
        c_pos = vector_subtract(c_pos, vector_scale(m, c_speed));
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        vec4 m = vector_cross(c_front, c_up);
        vector_normalize(&m);
        c_pos = vector_add(c_pos, vector_scale(m, c_speed));
    }

    if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS)
    {
        fb_test_pattern();
    }
    if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS)
    {
        fb_test_dot();
    }
    if (glfwGetKey(window, GLFW_KEY_F4) == GLFW_PRESS)
    {
        fb_drawSineWave(10, 3.1f, appTime);
    }
    if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS)
    {
        fb_copy_to_bg();
    }
}


Program *program_get_testmode()
{
    if (this != NULL) return this;
    this = malloc(sizeof(Program));

    this->init = testprogram_init;
    this->update = testprogram_update;
    this->destroy = testprogram_destroy;

    this->keyCallback = testprogram_keyCallback;
    this->mouseCallback = testprogram_mouseCallback;
    this->scrollCallback = testprogram_scrollCallback;
    
    return this;
}*/