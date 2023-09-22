#include "systems.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

Behaviour *behaviourVector;
int behaviourCapacity;
int behaviourTop;

int behaviourStatus[MAXBEHAVIOURS];
int behaviourIndex[MAXBEHAVIOURS];
functionPointerBehaviourPointer behaviourInitialVector[MAXBEHAVIOURS];

int behaviour_init()
{
    behaviourCapacity = 10;
    behaviourVector = (Behaviour*)malloc(behaviourCapacity * sizeof(Behaviour));
    behaviourTop = 0;
    memset(behaviourIndex, -1, sizeof(behaviourIndex));
    return 0;
}

Behaviour *behaviour_get_next()
{
    if (behaviourTop == behaviourCapacity)
    {
        behaviourCapacity *= 2;
        behaviourVector = (Behaviour*)realloc(behaviourVector, behaviourCapacity * sizeof(Behaviour));
    }
    behaviourTop++;
    return behaviourVector + behaviourTop;
}

int behaviour_update(float deltatime)
{
    if(programTop == -1) return 1;
    int i;
    for(i = 0; i < programTop; i++)
    {
        Behaviour *b = behaviourVector + i;
        if (!b->active) continue;
        if (b->update == NULL) continue;
        b->update(deltatime);
    }
    return 1;
}

Behaviour *behaviour_get(int id)
{
    Behaviour *b = NULL;
    if(behaviourIndex[id] == -1)
    {
        b = behaviour_get_next();
        behaviourStatus[id] = behaviourInitialVector[id](b);
        behaviourIndex[id] = behaviourTop;
    }
    else
    {
        int i = behaviourIndex[id];
        b = behaviourVector + i;
    }

    return b;
}