#ifndef systems_h
#define systems_h


typedef int (*functionPointer)();
typedef int (*functionPointerFloat)(float);
typedef int (*functionPointerInt)(int);
typedef int (*functionPointerDoubleDouble)(double, double);
typedef int (*functionPointerIntInt)(int, int);

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

typedef struct
{
    functionPointer init;
    functionPointerFloat update;
    functionPointer destroy;
    functionPointerIntInt keyCallback;
    functionPointerDoubleDouble mouseCallback;
    functionPointerDoubleDouble scrollCallback;
}Program;

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
Program *program_get_testmode();
Program *program_get_boidmode();
Program *program_get_selftest();

/* BEHAVIOURS */

#define MAXBEHAVIOURS 100

typedef struct
{
    /* -- base -- */
    functionPointer add;
    functionPointer remove;
    char active;
    unsigned char type;
    /* -- instance -- */
    functionPointerInt init;
    functionPointerFloat update;
    functionPointerInt destroy;
    void *dataStore;
    unsigned int *entities;
    int instanceCount;
    int instanceTop;
}Behaviour;

typedef int (*functionPointerBehaviourPointer)(Behaviour*);

int behaviour_init(void);
int behaviour_update(float deltaTime);
Behaviour *behaviour_get(int id);

#endif