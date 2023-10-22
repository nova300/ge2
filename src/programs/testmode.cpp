#include "testmode.h"
#include "hlsl++.h"

void TestMode::init()
{
    glfwSetWindowTitle(window, "test");

    terminal_clear();
    terminal_print("test program no.1 loaded\n");

    sensitivity = 0.1f;
    speed = 2.0f;
    firstMouse = true;
    lastX = SCREEN_WIDTH / 2;
    lastY = SCREEN_HEIGHT / 2;
    yaw = -90.0f;
    pitch = 0.0f;
    captureMouse = 0;

    float4 eye = float4(0, 0, 5, 0);
    float4 center = float4(0, 0, -1, 0);

    c_pos = eye;
    c_front = center;

    skybox_load_texture("media/bitfs2.png");

    GeoObject *gobj = geo_new_object();
    geo_obj_loadFromFile("media/cube.obj", gobj);

    rq_init(&renderQueue1, 10);

    Shader *s = newShaderObject(vertex_shader_0, fragment_shader_0);
    //gobj->shader = s;
    renderQueue1.gpuHandle.shader = s;

    rq = renderQueue1.objectBuffer;

    renderQueue1.gpuHandle.textureAtlas = generateRandomAtlas();

    gobj->baseTexture = 5;
    rq_add_object(&renderQueue1, gobj);

    //GeoObject *cube1 = geo_new_object();
    //geo_obj_loadFromFile("media/cube.obj", cube1);

    //rq_add_object(&renderQueue1, cube1);

    vertex_c *verices = (vertex_c*)malloc(sizeof(vertex_c) * 3);

    verices[0] = gfx_make_color_vertex(1, 1, -1, 1, 1, gfx_make_color(255, 0, 0, 255));
    verices[1] = gfx_make_color_vertex(1, 0, -1, 1, 1, gfx_make_color(0, 255, 0, 255));
    verices[2] = gfx_make_color_vertex(0, 0, -1, 1, 1, gfx_make_color(0, 0, 255, 255));

    int *indices = (int*)malloc(sizeof(int) * 3);
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;

    GeoObject *gobj2 = geo_new_object();

    gobj2->type = GOBJ_TYPE_COLOR;
    gobj2->data = (vertex*)verices;
    gobj2->dataCount = 3;
    gobj2->indicies = indices;
    gobj2->indexCount = 3;
    
    obj2 = geo_obj_bindToGpu(*gobj2);

    geo_obj_gpu_updateBuffers(obj2);

    obj2->gpuHandle.shader = s;
    obj2->gpuHandle.textureAtlas = generateRandomAtlas();


}

void TestMode::update(float deltaTime)
{
    key_input_poll();
    //transform_rotate(0, 0, 1 * deltaTime, &rq[1]->baseTransform);
    //transform_rotate(1 * deltaTime, 0, 0, &rq[2]->baseTransform);

    // particle_update(p1);

    //rq[2]->baseTexture = rand() % 50;
    //rq[2]->instanceDirty = 1;

    rq_update_buffers(&renderQueue1);

    render_skybox();

    geo_render(&obj2->gpuHandle);

    //geo_render(&renderQueue1.gpuHandle);
}

void TestMode::destroy()
{
    freeShaderObject(renderQueue1.gpuHandle.shader);
    glDeleteTextures(1, &renderQueue1.gpuHandle.textureAtlas);
    rq_free_with_objects(&renderQueue1);
}

void TestMode::keyCallback(int key, int action)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        program_pop();
    }
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
    {
        if (captureMouse)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            captureMouse = 0;
            return;
        }
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        captureMouse = 1;
    }
}

void TestMode::mouseCallback(double xpos, double ypos)
{
    if (!captureMouse)
    {
        return;
    }
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
  
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 
    lastX = xpos;
    lastY = ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f) pitch = 89.0f;
    if(pitch < -89.0f) pitch = -89.0f;

    float4 direction;
    direction.x = cos(radians(yaw)) * cos(radians(pitch));
    direction.y = sin(radians(pitch));
    direction.z = sin(radians(yaw)) * cos(radians(pitch));
    direction.w = 0;
    vector_normalize(&direction);
    c_front = direction;
}

void TestMode::scrollCallback(double xoffset, double yoffset)
{
    fov = fov - (yoffset * 10);
    if (fov < 1.0f) fov = 1.0f;
    if (fov > 120.0f) fov = 120.0f; 
    float4x4 m = matrix_perspective(radians(fov), (float)s_width/s_height, 0.1f, 100.0f);
    store(m, (float*)&projectionMatrix);
}

void TestMode::key_input_poll(void)
{
    float c_speed = speed * deltaTime;
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
        float4 m = vector_cross(c_front, c_up);
        vector_normalize(&m);
        c_pos = vector_subtract(c_pos, vector_scale(m, c_speed));
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        float4 m = vector_cross(c_front, c_up);
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