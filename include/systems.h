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
    virtual int init();
    virtual int update(float deltaTime);
    virtual int destroy();
    virtual int keyCallback(int x, int y);
    virtual int mouseCallback(double x, double y);
    virtual int scrollCallback(double x, double y);
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

/* BEHAVIOURS */

#define MAXBEHAVIOURS 100

class Behaviour
{
public:
    /* -- base -- */
    void add();
    void remove();
    char active;
    unsigned char type;
    /* -- instance -- */
    void init(int);
    void update(float deltaTime);
    void destroy(int);
    void *dataStore;
    unsigned int *entities;
    int instanceCount;
    int instanceTop;
};

#endif