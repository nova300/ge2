#ifndef testmode_h
#define testmode_h

#include "engine.h"
#include "shaders.h"

#include "term.h"
#include "systems.h"

class TestMode : public Program
{
public:
    void init();
    void update(float deltaTime);
    void destroy();
    void keyCallback(int x, int y);
    void mouseCallback(double x, double y);
    void scrollCallback(double x, double y);
private:
    void key_input_poll(void);
    
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
};



#endif