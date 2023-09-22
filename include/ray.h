#ifndef ray_h
#define ray_h

#include <SDL2/SDL.h>

SDL_Texture* raycast(SDL_Renderer *rend);

float dist(float ax, float ay, float bx, float by, float ang);

void calcDeltas();

void pan(int dir);

void move(int dir);

#endif