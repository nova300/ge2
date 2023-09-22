#ifdef ray_h
#include "ray.h"
#include <math.h>

float pdx = 0;
float pdy = 0;
float panSpeed = 0.01;
int mapX=9, mapY=8, mapS=64;

char shadow = 0;
float playerAngle;
float playerX = 300, playerY = 300;
float xOffset, yOffset;
float dgr = 0.004363325;
int max;

#define PI 3.14

const float pi = PI;
const float pi2 = PI/2;
const float pi3 = 3*PI/2;
    
unsigned char map[]=         /* map array */
{
    1,1,1,1,1,1,1,1,1,
    1,0,0,0,0,1,0,0,1,
    1,1,1,1,0,0,0,0,1,
    1,0,0,1,0,0,0,1,1,
    1,0,0,0,0,0,0,0,1,
    1,0,1,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,1,
    1,1,1,1,1,1,1,1,1,	
};

float dist(float ax, float ay, float bx, float by, float ang)
{
    return sqrtf((bx-ax) * (bx-ax) + (by-ay) * (by-ay));
}

void calcDeltas()
{
    pdx = cos(playerAngle) * 5;
    pdy = sin(playerAngle) * 5;
}

void pan(int dir)
{
    if (dir == 0)
    {
        playerAngle -= panSpeed;
        if (playerAngle < 0)
        {
            playerAngle += 2 * pi;
        }
        calcDeltas();
    }
    else
    {
        playerAngle += panSpeed;
        if (playerAngle > 2 * pi)
        {
            playerAngle -= 2 * pi;
        }
        calcDeltas();
    }
}

void move(int dir)
{
    if (dir == 0)
    {
        playerX += pdx / 4;
        playerY += pdy / 4;
    }
    else
    {
        playerX -= pdx / 4;
        playerY -= pdy / 4;
    }
}



SDL_Texture* raycast(SDL_Renderer *rend)
{

    SDL_Surface *s = SDL_CreateRGBSurface(0,1024,768,32,0,0,0,0);;
    SDL_Renderer *sr = SDL_CreateSoftwareRenderer(s);

    SDL_SetRenderDrawColor(sr, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(sr);

    float rayX=0,rayY=0,disT=0;
    float rayAngle = playerAngle - dgr * 180;

    for(int r = 0; r < 360; r++)
    {
        max = 0;
        float disH = 10000;
        float horizX = playerX;
        float horizY = playerY;

        float aTan = (-1/tanf(rayAngle));
        if (rayAngle > pi)      /* if the rayangle is more than pi we are looking up */
        {
            rayY = (((int) playerY / mapS) * mapS) -0.0001f;
            rayX = (playerY - rayY) * aTan + playerX;
            yOffset =- mapS;
            xOffset =- yOffset * aTan;
        }
        else if (rayAngle < pi)
        {
            rayY = (((int)(playerY/mapS) * mapS) + mapS);
            rayX = (playerY - rayY) * aTan + playerX;
            yOffset = mapS;
            xOffset = -yOffset * aTan;
        }
        else
        {
            rayX = playerX;
            rayY = playerY;
            max = 8;
        }
        while (max < 8)
        {
            int mx = (int) (rayX) / mapS;
            int my = (int) (rayY) / mapS;
            int mp = my * mapX + mx;
            if (mp > 0 && mp < (mapX * mapY) && map[mp] == 1)
            {
                disH = dist(rayX, rayY, playerX, playerY, rayAngle);
                horizX = rayX;
                horizY = rayY;
                max = 8;
            }
            else
            {
                rayX += xOffset;
                rayY += yOffset;
                max += 1;
            }
        }


        max = 0;
        float disV = 1000;
        float vertX = playerX;
        float vertY = playerY;
        float nTan = (-tanf(rayAngle));
        if ( rayAngle > pi2 && rayAngle < pi3)
        {
            rayX = (((int) playerX / mapS) * mapS) -0.0001f;
            rayY = (playerX - rayX) * nTan + playerY;
            xOffset =- mapS;
            yOffset =- xOffset * nTan;
        }
        else if( rayAngle < pi2 || rayAngle > pi3)
        {
            rayX = (((int) playerX / mapS) * mapS) + mapS;
            rayY = (playerX - rayX) * nTan + playerY;
            xOffset = mapS;
            yOffset =- xOffset * nTan;
        }
        while (max < 8)
        {
            int mx = (int) (rayX) / mapS;
            int my = (int) (rayY) / mapS;
            int mp = my * mapX + mx;
            if (mp > 0 && mp <(mapX*mapY) && map[mp] == 1)
            {
                disV = dist(rayX, rayY, playerX, playerY, rayAngle);
                vertX = rayX;
                vertY = rayY;
                max = 8;
            }
            else 
            {
                rayX += xOffset;
                rayY += yOffset;
                max += 1;
            }
        }



        if (disV > disH)
        {
            rayX = horizX;
            rayY = horizY;
            disT = disH;
            shadow = 0;
        }

        if (disV < disH)
        {
            rayX = vertX;
            rayY = vertY;
            disT = disV;
            shadow = 1;
        }



        float cameraAngle = playerAngle - rayAngle;
        if (cameraAngle < 0)
        {
            cameraAngle -= 2*pi;
        }
        if (cameraAngle < 2 * pi)
        {
            cameraAngle -= 2 * pi;
        }

        disT = disT * cosf(cameraAngle);

        float lineH = (64 * 320) / disT;
        if (lineH > 320)
        {
            lineH = 320;
        }

        float lineOffset = 250 - lineH / 2;

        if(shadow)
        {
            SDL_SetRenderDrawColor( sr, 0x00, 0x7f, 0x7f, 0x7f);
        }
        else
        {
            SDL_SetRenderDrawColor( sr, 0x00, 0xff, 0xff, 0xff);
        }
        SDL_RenderDrawLine(sr, r*2+100, lineOffset, r*2+100, lineH + lineOffset);
        SDL_RenderDrawLine(sr, r*2+100+1, lineOffset, r*2+100+1, lineH + lineOffset);

        rayAngle += dgr;
        if (rayAngle < 0)
        {
            rayAngle += 2 * pi;
        }
        if (rayAngle > 2 * pi)
        {
            rayAngle -= 2 * pi;
        }
    }

    SDL_RenderPresent(sr);
    SDL_Texture *t = NULL;
    t = SDL_CreateTextureFromSurface(rend, s);
    SDL_DestroyRenderer(sr);
    SDL_FreeSurface(s);
    return t;
}

#endif