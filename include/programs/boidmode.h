#ifndef boidmode_h
#define boidmode_h

#include "engine.h"
#include "shaders.h"

#include "term.h"
#include "systems.h"

//#include "thread.h"
//#include <pthread.h>
//#include <stdatomic.h>

struct boid
{
    boid **localBoidList;
    int localBoidListAmount;
    Transform transform;
    vec4 direction;
    float radius;
    float steerSpeed;
    float speed;
    int texture;
    int id;
};

class BoidMode : public Program
{
public:
    int init();
    int update(float deltaTime);
    int destroy();
    int keyCallback(int x, int y);
    int mouseCallback(double x, double y);
    int scrollCallback(double x, double y);
private:

    void *update_boids(void* arg);

    void doCohesion(boid *b);
    void doAlignment(boid *b);
    void doSeperation(boid *b);
    void doRetention(boid *b);

    void key_input_poll(void);

    void updateLocalBoidList(boid *b);
    
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
    bool threadStatus;
};



#endif