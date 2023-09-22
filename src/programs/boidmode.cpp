#include "engine.h"
#include "shaders.h"

#include "term.h"
#include "systems.h"

//#include "thread.h"
#include <pthread.h>
#include <stdatomic.h>


static Program *this = NULL;

void boidprogram_key_input_poll(void);

typedef struct Boid
{
    struct Boid **localBoidList;
    int localBoidListAmount;
    Transform transform;
    vec4 direction;
    float radius;
    float steerSpeed;
    float speed;
    int texture;
    int id;
}boid;

typedef struct
{
    GeoObject **rq;
    RenderQueue renderQueue1;
    float sensitivity;
    float mspeed;
    char firstMouse;
    float lastX;
    float lastY;
    float yaw;
    float pitch;
    char captureMouse;
    boid *boids;
    Transform *transform;
    int amount;
    float speed;
    float steerSpeed;
    float radius;
    float separationWeight;
    float alignmentWeight;
    char scrollmode;
    vec4 zero;
    GeoObject *gobj;
    float fpstimer;
    float fps;
    int fpscounter;
    pthread_t thread;
    atomic_int threadStatus;
}boidmode_localstorage;

static boidmode_localstorage *localstorage;

void *update_boids(void* arg);


int boidprogram_init()
{   

    glfwSetWindowTitle(window, "boids");

    terminal_clear();
    terminal_print("boid program loaded\n");

    fb_load_bg("media/underwater2.png", true);

    localstorage = malloc(sizeof(boidmode_localstorage));

    localstorage->sensitivity = 0.1f;
    localstorage->mspeed = 2.0f;
    localstorage->firstMouse = true;
    localstorage->lastX = SCREEN_WIDTH / 2;
    localstorage->lastY = SCREEN_HEIGHT / 2;
    localstorage->yaw = -90.0f;
    localstorage->pitch = 0.0f;
    localstorage->captureMouse = 0;
    localstorage->separationWeight = 0.30;
    localstorage->alignmentWeight = 0.145;
    localstorage->scrollmode = 0;
    localstorage->fpstimer = 0;
    localstorage->fps = 999;
    localstorage->fpscounter = 0;
    localstorage->threadStatus = 0;

    vec4 eye = {{0, 0, 50, 0}};
    vec4 center = {{0, 0, -1, 0}};

    c_pos = eye;
    c_front = center;

    pthread_create(&localstorage->thread, NULL, update_boids, NULL);

    localstorage->gobj = geo_new_object();
    geo_obj_loadFromFile("media/cube.obj", localstorage->gobj);

    rq_init(&localstorage->renderQueue1, 10);


    Shader *s = newShaderObject(vertex_shader_0, fragment_shader_0);
    localstorage->renderQueue1.gpuHandle.shader = s;

    localstorage->rq = localstorage->renderQueue1.objectBuffer;

    transform_set_identity(&localstorage->gobj->baseTransform);

    localstorage->renderQueue1.gpuHandle.textureAtlas = generateRandomAtlas();

    localstorage->gobj->baseTexture = 5;

    rq_add_object(&localstorage->renderQueue1, localstorage->gobj);

    //p1 = particle_new(gobj, 100);

    localstorage->amount = 2000;

    int len = snprintf(NULL, 0, "%d", localstorage->amount);
    char *result = malloc(len + 1);
    snprintf(result, len + 1, "%d", localstorage->amount);
    terminal_print(result);
    terminal_print(" boids\n");
    free(result);

    localstorage->radius = 10.0f;
    localstorage->steerSpeed = 0.01f;
    localstorage->speed = 10.0f;

    geo_instanceop_init(localstorage->gobj, localstorage->amount);

    localstorage->transform = &(localstorage->gobj->baseTransform);
    boid *b = malloc(sizeof(boid) * localstorage->amount);

    localstorage->zero.x = 0;
    localstorage->zero.y = 0;
    localstorage->zero.z = 0;
    localstorage->zero.w = 0;

    for (int i = 0; i < localstorage->amount; i++)
    {
        b[i].radius = localstorage->radius;
        b[i].speed = localstorage->speed;
        b[i].steerSpeed = localstorage->steerSpeed;
        b[i].direction.x = ((rand() - rand()) % 3) + ((rand() - rand()) % 10);
        b[i].direction.y = ((rand() - rand()) % 3) + ((rand() - rand()) % 10);
        b[i].direction.z = ((rand() - rand()) % 3) + ((rand() - rand()) % 10);
        b[i].texture = rand() % 50;
        transform_set_identity(&b[i].transform);

        transform_position(((rand() - rand()) % 3) + ((rand() - rand()) % 10), ((rand() - rand()) % 3) + ((rand() - rand()) % 10), ((rand() - rand()) % 3) + ((rand() - rand()) % 10), &b[i].transform);
        
        b[i].localBoidList = malloc(sizeof(boid*) * 20);
        b[i].localBoidListAmount = 0;

        b[i].id = i;

    }
    
    localstorage->boids = b;
}

void updateLocalBoidList(boid *b)
{
    b->localBoidListAmount = 0;

    for (int i = 0; i < localstorage->amount; i++)
    {
        if (b->localBoidListAmount == 20) break;
        if (localstorage->boids[i].id == b->id) continue;

        if (vector_distance(b->transform.position, localstorage->boids[i].transform.position) < localstorage->radius)
        {
            b->localBoidList[b->localBoidListAmount] = &localstorage->boids[i];
            b->localBoidListAmount++;
        }
    }
}

void doCohesion(boid *b)
{
    vec4 avg = localstorage->zero;
    if (b->localBoidListAmount == 0) return;

    for (int i = 0; i < b->localBoidListAmount; i++)
    {
        avg = vector_add(avg, b->localBoidList[i]->transform.position);
    }

    avg.x = avg.x / b->localBoidListAmount;
    avg.y = avg.y / b->localBoidListAmount;
    avg.z = avg.z / b->localBoidListAmount;

    vec4 dir = vector_subtract(avg, b->transform.position);

    vector_normalize(&dir);

    b->direction.x += dir.x * localstorage->steerSpeed;
    b->direction.y += dir.y * localstorage->steerSpeed;
    b->direction.z += dir.z * localstorage->steerSpeed;
}

void doAlignment(boid *b)
{
    vec4 avg = localstorage->zero;
    if (b->localBoidListAmount == 0) return;

    for (int i = 0; i < b->localBoidListAmount; i++)
    {
        avg = vector_add(avg, b->localBoidList[i]->direction);
    }

    avg.x = avg.x / b->localBoidListAmount;
    avg.y = avg.y / b->localBoidListAmount;
    avg.z = avg.z / b->localBoidListAmount;

    float alignSpeed = localstorage->steerSpeed * localstorage->alignmentWeight;

    b->direction.x += avg.x * alignSpeed;
    b->direction.y += avg.y * alignSpeed;
    b->direction.z += avg.z * alignSpeed;
}

void doSeperation(boid *b)
{
    float sepRad = localstorage->radius * 0.8f;
    //boid *closeBoids[50];
    int closeBoidsAmount = 0;
    vec4 avg = localstorage->zero;
    
    for (int i = 0; i < b->localBoidListAmount; i++)
    {
        float dist = vector_distance(b->transform.position, b->localBoidList[i]->transform.position);
        if (dist < sepRad)
        {
            //closeBoids[closeBoidsAmount] = &boids[i];
            closeBoidsAmount++;
            //sepRad = dist;

            vec4 diff;

            diff.x = b->localBoidList[i]->transform.position.x;
            diff.y = b->localBoidList[i]->transform.position.y;
            diff.z = b->localBoidList[i]->transform.position.z;

            /*if (dist > 1.0f)
            {
                dist = 1.0f;
            }

            diff.x = diff.x - (diff.x * dist);
            diff.y = diff.y - (diff.y * dist);
            diff.z = diff.z - (diff.z * dist); */

            avg = vector_add(avg, diff);
        }
    }

    if (closeBoidsAmount == 0)
    {
        return;
    }

    avg.x = avg.x / closeBoidsAmount;
    avg.y = avg.y / closeBoidsAmount;
    avg.z = avg.z / closeBoidsAmount;

    vec4 dir = vector_subtract(avg, b->transform.position);

    float separationSpeed = localstorage->steerSpeed * localstorage->separationWeight;

    b->direction.x -= dir.x * separationSpeed;
    b->direction.y -= dir.y * separationSpeed;
    b->direction.z -= dir.z * separationSpeed;
}

void doRetention(boid *b)
{
    float retentionDist = 40.0f;
    if (vector_distance(localstorage->transform->position, b->transform.position) > (retentionDist * 2))
    {
        transform_position(0.0f, 0.0f, 0.0f, &b->transform);
    }

    if (vector_distance(localstorage->transform->position, b->transform.position) > retentionDist)
    {
        vec4 dir = vector_subtract(b->transform.position, localstorage->transform->position);

        b->direction.x -= dir.x * localstorage->steerSpeed * 2;
        b->direction.y -= dir.y * localstorage->steerSpeed * 2;
        b->direction.z -= dir.z * localstorage->steerSpeed * 2;
    }
}

void *update_boids(void* arg)
{
    while (true)
    {
        pthread_testcancel();
        boid *boids = localstorage->boids;
        if (localstorage->threadStatus == 1)
        {
            int i;
            #pragma omp parallel for
            for (i = 0; i < localstorage->amount; i++)
            {
                updateLocalBoidList(&localstorage->boids[i]);
                doAlignment(&boids[i]);
                doCohesion(&boids[i]);
                doSeperation(&boids[i]);
                doRetention(&boids[i]);
                vector_normalize(&boids[i].direction);
                transform_move(boids[i].direction.x * (deltaTime * localstorage->speed), boids[i].direction.y * (deltaTime * localstorage->speed), boids[i].direction.z * (deltaTime * localstorage->speed), &boids[i].transform);
                transform_set_rotation(boids[i].direction.x, boids[i].direction.y, boids[i].direction.z, &boids[i].transform);
                transform_make_matrix(&boids[i].transform);
            }
            localstorage->threadStatus = 0;
        }
    }
}

int boidprogram_update(float deltaTime)
{
    if (localstorage == NULL) return 0;
    boidprogram_key_input_poll();

    if (localstorage->threadStatus == 0)
    {
        
        geo_instanceop_clear(localstorage->gobj);
        for (int i = 0; i < localstorage->amount; i++)
        {
            geo_instanceop_add(localstorage->gobj, localstorage->boids[i].transform.matrix, localstorage->boids[i].texture);
        }
        rq_update_buffers(&localstorage->renderQueue1);

        localstorage->threadStatus = 1;

    }

    geo_render(&localstorage->renderQueue1.gpuHandle);



    if (localstorage->fpstimer > 1)
    {
        localstorage->fps = localstorage->fpstimer / localstorage->fpscounter;
        localstorage->fps = 1 / localstorage->fps;
        localstorage->fpscounter = 0;
        localstorage->fpstimer = 0;
    }
    else
    {
        localstorage->fpstimer = localstorage->fpstimer + deltaTime;
        localstorage->fpscounter++;
    }
    int len = snprintf(NULL, 0, "%3.0f", localstorage->fps);
    char *result = malloc(len + 1);
    snprintf(result, len + 1, "%3.0f", localstorage->fps);
    terminal_display(result);
    free(result);
}

int boidprogram_destroy()
{
    pthread_cancel(localstorage->thread);
    pthread_join(localstorage->thread, NULL);
    localstorage->threadStatus = 0;
    for (int i = 0; i < localstorage->amount; i++)
    {   
        free(localstorage->boids[i].localBoidList);
    }
    rq_free(&localstorage->renderQueue1);
    freeShaderObject(localstorage->renderQueue1.gpuHandle.shader);
    glDeleteTextures(1, &localstorage->renderQueue1.gpuHandle.textureAtlas);
    geo_instanceop_free(localstorage->gobj);
    geo_obj_free(localstorage->gobj);
    free(this);
    free(localstorage->boids);
    free(localstorage);
    this = NULL;
    localstorage == NULL;
}

int boidprogram_keyCallback(int key, int action)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        program_pop();
    }
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
    {
       for (int i = 0; i < localstorage->amount; i++)
        {
            transform_position(((rand() - rand()) % 30) + ((rand() - rand()) % 100), ((rand() - rand()) % 30) + ((rand() - rand()) % 100), ((rand() - rand()) % 30) + ((rand() - rand()) % 100), &localstorage->boids[i].transform);
        } 
        terminal_clear();
        terminal_print("reset simulation\n");
    }
}

int boidprogram_mouseCallback(double xpos, double ypos)
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

int boidprogram_scrollCallback(double xoffset, double yoffset)
{
    if (localstorage->scrollmode == 0)
    {
        fov = fov - (yoffset * 10);
        if (fov < 1.0f)
        fov = 1.0f;
        if (fov > 120.0f)
        fov = 120.0f;
        projectionMatrix = matrix_perspective(radians(fov), (float)s_width / s_height, 0.1f, 100.0f);
        return 0;
    }
    if (localstorage->scrollmode == 1)
    {
        localstorage->alignmentWeight += (yoffset * 0.01f);
        printf("alignment weight: %f\n", localstorage->alignmentWeight);
    }

    if (localstorage->scrollmode == 2)
    {
        localstorage->separationWeight += (yoffset * 0.01f);
        printf("separation weight: %f\n", localstorage->separationWeight);
    }
}

void boidprogram_key_input_poll(void)
{
    float c_speed = localstorage->mspeed * deltaTime;
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
        localstorage->scrollmode = 0;
    }
    if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS)
    {
        localstorage->scrollmode = 1;
    }
    if (glfwGetKey(window, GLFW_KEY_F4) == GLFW_PRESS)
    {
        localstorage->scrollmode = 2;
    }
    if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS)
    {
        localstorage->scrollmode = 3;
    }
}


Program *program_get_boidmode()
{
    if (this != NULL) return this;
    this = malloc(sizeof(Program));

    this->init = boidprogram_init;
    this->update = boidprogram_update;
    this->destroy = boidprogram_destroy;

    this->keyCallback = boidprogram_keyCallback;
    this->mouseCallback = boidprogram_mouseCallback;
    this->scrollCallback = boidprogram_scrollCallback;

    return this;
}