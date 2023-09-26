#include "boidmode.h"

void BoidMode::init()
{   

    glfwSetWindowTitle(window, "boids");

    terminal_clear();
    terminal_print("boid program loaded\n");

    fb_load_bg("media/underwater2.png", true);

    sensitivity = 0.1f;
    mspeed = 2.0f;
    firstMouse = true;
    lastX = SCREEN_WIDTH / 2;
    lastY = SCREEN_HEIGHT / 2;
    yaw = -90.0f;
    pitch = 0.0f;
    captureMouse = 0;
    separationWeight = 0.30;
    alignmentWeight = 0.145;
    scrollmode = 0;
    fpstimer = 0;
    fps = 999;
    fpscounter = 0;
    threadStatus = 0;
    runThread = true;

    boidThread = new std::thread(update_boids, this);

    float4 eye = float4(0, 0, 50, 0);
    float4 center = float4(0, 0, -1, 0);

    c_pos = eye;
    c_front = center;

    par_shapes_mesh *mesh1 = par_shapes_create_tetrahedron();
    par_shapes_compute_normals(mesh1);
    gobj = geo_obj_createFromParShape(mesh1);
    par_shapes_free_mesh(mesh1);

    rq_init(&this->renderQueue1, 10);


    Shader *s = newShaderObject(vertex_shader_0, fragment_shader_0);
    this->renderQueue1.gpuHandle.shader = s;

    this->rq = this->renderQueue1.objectBuffer;

    transform_set_identity(&this->gobj->baseTransform);

    this->renderQueue1.gpuHandle.textureAtlas = generateRandomAtlas();

    this->gobj->baseTexture = 5;

    rq_add_object(&this->renderQueue1, this->gobj);

    //p1 = particle_new(gobj, 100);

    this->amount = 2000;

    int len = snprintf(NULL, 0, "%d", this->amount);
    char *result = (char*)malloc(len + 1);
    snprintf(result, len + 1, "%d", this->amount);
    terminal_print(result);
    terminal_print(" boids\n");
    free(result);

    this->radius = 10.0f;
    this->steerSpeed = 0.01f;
    this->speed = 10.0f;

    geo_instanceop_init(this->gobj, this->amount);

    this->transform = &(this->gobj->baseTransform);
    boid *b = (boid*)malloc(sizeof(boid) * this->amount);

    this->zero.x = 0;
    this->zero.y = 0;
    this->zero.z = 0;
    this->zero.w = 0;

    for (int i = 0; i < this->amount; i++)
    {
        b[i].radius = this->radius;
        b[i].speed = this->speed;
        b[i].steerSpeed = this->steerSpeed;
        b[i].direction.x = ((rand() - rand()) % 3) + ((rand() - rand()) % 10);
        b[i].direction.y = ((rand() - rand()) % 3) + ((rand() - rand()) % 10);
        b[i].direction.z = ((rand() - rand()) % 3) + ((rand() - rand()) % 10);
        b[i].texture = rand() % 50;
        transform_set_identity(&b[i].transform);

        transform_position(((rand() - rand()) % 3) + ((rand() - rand()) % 10), ((rand() - rand()) % 3) + ((rand() - rand()) % 10), ((rand() - rand()) % 3) + ((rand() - rand()) % 10), &b[i].transform);
        
        b[i].localBoidList = (boid**)malloc(sizeof(boid*) * 20);
        b[i].localBoidListAmount = 0;

        b[i].id = i;

    }
    
    boids = b;
}

void BoidMode::updateLocalBoidList(boid *b)
{
    b->localBoidListAmount = 0;

    for (int i = 0; i < amount; i++)
    {
        if (b->localBoidListAmount == 20) break;
        if (boids[i].id == b->id) continue;

        if (vector_distance(b->transform.position, boids[i].transform.position) < radius)
        {
            b->localBoidList[b->localBoidListAmount] = &boids[i];
            b->localBoidListAmount++;
        }
    }
}

void BoidMode::doCohesion(boid *b)
{
    float4 avg = zero;
    if (b->localBoidListAmount == 0) return;

    for (int i = 0; i < b->localBoidListAmount; i++)
    {
        avg = vector_add(avg, b->localBoidList[i]->transform.position);
    }

    avg.x = avg.x / b->localBoidListAmount;
    avg.y = avg.y / b->localBoidListAmount;
    avg.z = avg.z / b->localBoidListAmount;

    float4 dir = vector_subtract(avg, b->transform.position);

    vector_normalize(&dir);

    b->direction.x = b->direction.x + (dir.x * steerSpeed);
    b->direction.y = b->direction.y + (dir.y * steerSpeed);
    b->direction.z = b->direction.z + (dir.z * steerSpeed);
}

void BoidMode::doAlignment(boid *b)
{
    float4 avg = zero;
    if (b->localBoidListAmount == 0) return;

    for (int i = 0; i < b->localBoidListAmount; i++)
    {
        avg = vector_add(avg, b->localBoidList[i]->direction);
    }

    avg.x = avg.x / b->localBoidListAmount;
    avg.y = avg.y / b->localBoidListAmount;
    avg.z = avg.z / b->localBoidListAmount;

    float alignSpeed = steerSpeed * alignmentWeight;

    b->direction.x += avg.x * alignSpeed;
    b->direction.y += avg.y * alignSpeed;
    b->direction.z += avg.z * alignSpeed;
}

void BoidMode::doSeperation(boid *b)
{
    float sepRad = radius * 0.8f;
    //boid *closeBoids[50];
    int closeBoidsAmount = 0;
    float4 avg = zero;
    
    for (int i = 0; i < b->localBoidListAmount; i++)
    {
        float dist = vector_distance(b->transform.position, b->localBoidList[i]->transform.position);
        if (dist < sepRad)
        {
            //closeBoids[closeBoidsAmount] = &boids[i];
            closeBoidsAmount++;
            //sepRad = dist;

            float4 diff;

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

    float4 dir = vector_subtract(avg, b->transform.position);

    float separationSpeed = steerSpeed * separationWeight;

    b->direction.x -= dir.x * separationSpeed;
    b->direction.y -= dir.y * separationSpeed;
    b->direction.z -= dir.z * separationSpeed;
}

void BoidMode::doRetention(boid *b)
{
    float retentionDist = 40.0f;
    if (vector_distance(transform->position, b->transform.position) > (retentionDist * 2))
    {
        transform_position(0.0f, 0.0f, 0.0f, &b->transform);
    }

    if (vector_distance(transform->position, b->transform.position) > retentionDist)
    {
        float4 dir = vector_subtract(b->transform.position, transform->position);

        b->direction.x -= dir.x * steerSpeed * 2;
        b->direction.y -= dir.y * steerSpeed * 2;
        b->direction.z -= dir.z * steerSpeed * 2;
    }
}

void BoidMode::update_boids(BoidMode* boidInstance)
{
    while (boidInstance->runThread)
    {
        //pthread_testcancel();
        boid *boids = boidInstance->boids;
        if (boidInstance->threadStatus == 1)
        {
            int i;
            #pragma omp parallel for
            for (i = 0; i < boidInstance->amount; i++)
            {
                boidInstance->updateLocalBoidList(&boids[i]);
                boidInstance->doAlignment(&boids[i]);
                boidInstance->doCohesion(&boids[i]);
                boidInstance->doSeperation(&boids[i]);
                boidInstance->doRetention(&boids[i]);
                vector_normalize(&boids[i].direction);
                transform_move(boids[i].direction.x * (deltaTime * boidInstance->speed), boids[i].direction.y * (deltaTime * boidInstance->speed), boids[i].direction.z * (deltaTime * boidInstance->speed), &boids[i].transform);
                transform_set_rotation(boids[i].direction.x, boids[i].direction.y, boids[i].direction.z, &boids[i].transform);
                transform_make_matrix(&boids[i].transform);
            }
            boidInstance->threadStatus = 0;
        }
    }
}

void BoidMode::update(float deltaTime)
{
    key_input_poll();

    if (threadStatus == 0)
    {
        
        geo_instanceop_clear(this->gobj);
        for (int i = 0; i < this->amount; i++)
        {
            geo_instanceop_add(this->gobj, this->boids[i].transform.matrix, this->boids[i].texture);
        }
        rq_update_buffers(&this->renderQueue1);

        threadStatus = 1;

    }

    geo_render(&this->renderQueue1.gpuHandle);



    if (fpstimer > 1)
    {
        fps = fpstimer / fpscounter;
        fps = 1 / fps;
        fpscounter = 0;
        fpstimer = 0;
    }
    else
    {
        fpstimer = fpstimer + deltaTime;
        fpscounter++;
    }
    int len = snprintf(NULL, 0, "%3.0f", fps);
    char *result = (char*)malloc(len + 1);
    snprintf(result, len + 1, "%3.0f", fps);
    terminal_display(result);
    free(result);
}

void BoidMode::destroy()
{
    runThread = false;
    boidThread->join();
    delete(boidThread);
    threadStatus = 0;
    for (int i = 0; i < amount; i++)
    {   
        free(boids[i].localBoidList);
    }
    rq_free(&renderQueue1);
    freeShaderObject(renderQueue1.gpuHandle.shader);
    glDeleteTextures(1, &renderQueue1.gpuHandle.textureAtlas);
    geo_instanceop_free(gobj);
    geo_obj_free(gobj);
    free(boids);
}

void BoidMode::keyCallback(int key, int action)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        program_pop();
    }
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
    {
       for (int i = 0; i < amount; i++)
        {
            transform_position(((rand() - rand()) % 30) + ((rand() - rand()) % 100), ((rand() - rand()) % 30) + ((rand() - rand()) % 100), ((rand() - rand()) % 30) + ((rand() - rand()) % 100), &boids[i].transform);
        } 
        terminal_clear();
        terminal_print("reset simulation\n");
    }
}

void BoidMode::mouseCallback(double xpos, double ypos)
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
    direction = normalize(direction);
    //vector_normalize(&direction);
    c_front = direction;
}

void BoidMode::scrollCallback(double xoffset, double yoffset)
{
    if (scrollmode == 0)
    {
        fov = fov - (yoffset * 10);
        if (fov < 1.0f)
        fov = 1.0f;
        if (fov > 120.0f)
        fov = 120.0f;
        float4x4 m = matrix_perspective(radians(fov), (float)s_width/s_height, 0.1f, 100.0f);
        store(m, (float*)&projectionMatrix);
        return;
    }
    if (scrollmode == 1)
    {
        alignmentWeight += (yoffset * 0.01f);
        printf("alignment weight: %f\n", alignmentWeight);
    }

    if (scrollmode == 2)
    {
        separationWeight += (yoffset * 0.01f);
        printf("separation weight: %f\n", separationWeight);
    }
}

void BoidMode::key_input_poll(void)
{
    float c_speed = mspeed * deltaTime;
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
        scrollmode = 0;
    }
    if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS)
    {
        scrollmode = 1;
    }
    if (glfwGetKey(window, GLFW_KEY_F4) == GLFW_PRESS)
    {
        scrollmode = 2;
    }
    if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS)
    {
        scrollmode = 3;
    }
}