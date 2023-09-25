#ifndef selftest_h
#define selftest_h

#include "engine.h"
#include "shaders.h"

#include "term.h"
#include "systems.h"

class SelfTest : public Program
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
    
    float countdown = 5;
    char reinit;
};



#endif