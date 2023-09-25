#ifndef systems_h
#define systems_h

/* TASKS */

int run_script(void* num);

typedef struct
{
    int cost;
    int programNr;
    char* callStruct;
    char* returnStruct;
}TaskHandle;

int task_catch(TaskHandle *task);

int task_throw(TaskHandle *task);

int task_queue(TaskHandle *task);


/* PROGRAMS */


class Program
{
public:
    virtual void init() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void destroy() = 0;
    virtual void keyCallback(int x, int y) = 0;
    virtual void mouseCallback(double x, double y) = 0;
    virtual void scrollCallback(double x, double y) = 0;
};

extern Program **programStack;
extern int programCapacity;
extern int programTop;

int program_init(void);
int program_free(void);
int program_push(Program *program);
int program_pop(void);
int program_update(float deltaTime);
Program *program_get(void);

/* program prototypes */
//Program *program_get_testmode();
//Program *program_get_boidmode();
//Program *program_get_selftest();

#endif