#ifndef boidmode_h
#define boidmode_h

#include "engine.h"
#include "shaders.h"

#include "term.h"
#include "systems.h"

#include <atomic>
#include <thread>

//#include "thread.h"
//#include <pthread.h>
//#include <stdatomic.h>

struct boid
{
    boid **localBoidList;
    int localBoidListAmount;
    Transform transform;
    float4 direction;
    float radius;
    float steerSpeed;
    float speed;
    int texture;
    int id;
};

class BoidMode : public Program
{
public:
    void init();
    void update(float deltaTime);
    void destroy();
    void keyCallback(int x, int y);
    void mouseCallback(double x, double y);
    void scrollCallback(double x, double y);
private:

    static void update_boids(BoidMode* boidInstance);

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
    float4 zero;
    GeoObject *gobj;
    float fpstimer;
    float fps;
    int fpscounter;
    std::atomic_bool threadStatus;
    std::thread *boidThread;
    std::atomic_bool runThread;
};



#endif