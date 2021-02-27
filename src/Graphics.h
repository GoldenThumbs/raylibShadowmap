#ifndef ENGINE_GRAPHICS_H
#define ENGINE_GRAPHICS_H

#include <raylib.h>

typedef struct Graphics
{
    char *windowTitle;
    int width;
    int height;
    void *window;
}Graphics;

typedef struct ShadowMap
{
    unsigned int id;
    int width;
    int height;
    Texture2D depth;
}ShadowMap;

int GRAPHICS_init(Graphics *graphics);
void GRAPHICS_free(Graphics *graphics);

ShadowMap LoadShadowMap(int width, int height);
void UnloadShadowMap(ShadowMap shadowMap);
void ShadowMapBegin(ShadowMap shadowMap);
void ShadowMapEnd();

#endif