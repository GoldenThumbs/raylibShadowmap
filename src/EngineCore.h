#ifndef ENGINE_CORE_H
#define ENGINE_CORE_H

#include "Graphics.h"

struct EngineOptions
{
    char *title;
    int width;
    int height;
};

typedef struct Engine
{
    int quit;
    Graphics graphics;
}Engine;

int ENGINE_init(Engine *engine, struct EngineOptions *options);
int ENGINE_free(Engine *engine);

#endif